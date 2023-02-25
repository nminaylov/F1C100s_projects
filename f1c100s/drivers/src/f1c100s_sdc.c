#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "f1c100s_sdc.h"
#include "f1c100s_gpio.h"
#include "f1c100s_clock.h"

static uint8_t sdc_transfer_command(uint32_t sdc_base, sdc_cmd_t* cmd, sdc_data_t* dat);
static uint8_t
    sdc_read_bytes(uint32_t sdc_base, uint32_t* buf, uint32_t blkcount, uint32_t blksize);
static uint8_t
    sdc_write_bytes(uint32_t sdc_base, uint32_t* buf, uint32_t blkcount, uint32_t blksize);
static uint8_t sdc_transfer_data(uint32_t sdc_base, sdc_cmd_t* cmd, sdc_data_t* dat);
static uint8_t sdc_update_clock(uint32_t sdc_base);

static uint8_t sdc_transfer_command(uint32_t sdc_base, sdc_cmd_t* cmd, sdc_data_t* dat) {
    uint32_t cmdval = SDC_START;
    uint32_t status = 0;
    int timeout     = 0;

    if(cmd->cmdidx == MMC_STOP_TRANSMISSION) {
        timeout = 10000;
        do {
            status = read32(sdc_base + SDC_STAR);
            if(!timeout--) {
                write32(sdc_base + SDC_GCTL, SDC_HARDWARE_RESET);
                write32(sdc_base + SDC_RISR, 0xFFFFFFFF);
                return 0;
            }
        } while(status & SDC_CARD_DATA_BUSY);
        return 1;
    }

    if(cmd->cmdidx == MMC_GO_IDLE_STATE) cmdval |= SDC_SEND_INIT_SEQUENCE;
    if(cmd->resptype & MMC_RESP_PRESENT) {
        cmdval |= SDC_RESP_EXPIRE;
        if(cmd->resptype & MMC_RESP_136) cmdval |= SDC_LONG_RESPONSE;
        if(cmd->resptype & MMC_RESP_CRC) cmdval |= SDC_CHECK_RESPONSE_CRC;
    }

    if(dat != NULL) {
        cmdval |= SDC_DATA_EXPIRE | SDC_WAIT_PRE_OVER;
        if(dat->flag & MMC_DATA_WRITE) cmdval |= SDC_WRITE;
    }

    if(cmd->cmdidx == MMC_WRITE_MULTIPLE_BLOCK || cmd->cmdidx == MMC_READ_MULTIPLE_BLOCK)
        cmdval |= SDC_SEND_AUTO_STOP;

    write32(sdc_base + SDC_CAGR, cmd->cmdarg);

    if(dat != NULL) write32(sdc_base + SDC_GCTL, read32(sdc_base + SDC_GCTL) | 0x80000000);
    write32(sdc_base + SDC_CMDR, cmdval | cmd->cmdidx);

    timeout = 10000;
    do {
        status = read32(sdc_base + SDC_RISR);
        if(!timeout-- || (status & SDC_INTERRUPT_ERROR_BIT)) {
            write32(sdc_base + SDC_GCTL, SDC_HARDWARE_RESET);
            write32(sdc_base + SDC_RISR, 0xFFFFFFFF);
            return 0;
        }
    } while(!(status & SDC_COMMAND_DONE));

    if(cmd->resptype & MMC_RESP_BUSY) {
        timeout = 10000;
        do {
            status = read32(sdc_base + SDC_STAR);
            if(!timeout--) {
                write32(sdc_base + SDC_GCTL, SDC_HARDWARE_RESET);
                write32(sdc_base + SDC_RISR, 0xFFFFFFFF);
                return 0;
            }
        } while(status & SDC_CARD_DATA_BUSY);
    }

    if(cmd->resptype & MMC_RESP_136) {
        cmd->response[0] = read32(sdc_base + SDC_RESP3);
        cmd->response[1] = read32(sdc_base + SDC_RESP2);
        cmd->response[2] = read32(sdc_base + SDC_RESP1);
        cmd->response[3] = read32(sdc_base + SDC_RESP0);
    } else {
        cmd->response[0] = read32(sdc_base + SDC_RESP0);
    }
    write32(sdc_base + SDC_RISR, 0xFFFFFFFF);
    return 1;
}

static uint8_t
    sdc_read_bytes(uint32_t sdc_base, uint32_t* buf, uint32_t blkcount, uint32_t blksize) {
    uint64_t count = blkcount * blksize;
    uint32_t* tmp  = buf;
    uint32_t status, err, done;

    status = read32(sdc_base + SDC_STAR);
    err    = read32(sdc_base + SDC_RISR) & SDC_INTERRUPT_ERROR_BIT;
    while((!err) && (count >= sizeof(uint32_t))) {
        if(!(status & SDC_FIFO_EMPTY)) {
            *(tmp) = read32(sdc_base + SDC_FIFO);
            tmp++;
            count -= sizeof(uint32_t);
        }
        status = read32(sdc_base + SDC_STAR);
        err    = read32(sdc_base + SDC_RISR) & SDC_INTERRUPT_ERROR_BIT;
    }

    do {
        status = read32(sdc_base + SDC_RISR);
        err    = status & SDC_INTERRUPT_ERROR_BIT;
        if(blkcount > 1)
            done = status & SDC_AUTO_COMMAND_DONE;
        else
            done = status & SDC_DATA_OVER;
    } while(!done && !err);

    if(err & SDC_INTERRUPT_ERROR_BIT) return 0;
    write32(sdc_base + SDC_RISR, 0xFFFFFFFF);

    if(count > 0) return 0;
    return 1;
}

static uint8_t
    sdc_write_bytes(uint32_t sdc_base, uint32_t* buf, uint32_t blkcount, uint32_t blksize) {
    uint64_t count = blkcount * blksize;
    uint32_t* tmp  = buf;
    uint32_t status, err, done;

    status = read32(sdc_base + SDC_STAR);
    err    = read32(sdc_base + SDC_RISR) & SDC_INTERRUPT_ERROR_BIT;
    while(!err && (count > 0)) {
        if(!(status & SDC_FIFO_FULL)) {
            write32(sdc_base + SDC_FIFO, *tmp);
            tmp++;
            count -= sizeof(uint32_t);
        }
        status = read32(sdc_base + SDC_STAR);
        err    = read32(sdc_base + SDC_RISR) & SDC_INTERRUPT_ERROR_BIT;
    }

    do {
        status = read32(sdc_base + SDC_RISR);
        err    = status & SDC_INTERRUPT_ERROR_BIT;
        if(blkcount > 1)
            done = status & SDC_AUTO_COMMAND_DONE;
        else
            done = status & SDC_DATA_OVER;
    } while(!done && !err);

    if(err & SDC_INTERRUPT_ERROR_BIT) return 0;
    write32(sdc_base + SDC_GCTL, read32(sdc_base + SDC_RISR) | SDC_FIFO_RESET);
    write32(sdc_base + SDC_RISR, 0xFFFFFFFF);

    if(count > 0) return 0;
    return 1;
}

static uint8_t sdc_transfer_data(uint32_t sdc_base, sdc_cmd_t* cmd, sdc_data_t* dat) {
    uint32_t dlen = (uint32_t)(dat->blkcnt * dat->blksz);
    uint8_t ret   = 0;

    write32(sdc_base + SDC_BKSR, dat->blksz);
    write32(sdc_base + SDC_BYCR, dlen);
    if(dat->flag & MMC_DATA_READ) {
        if(!sdc_transfer_command(sdc_base, cmd, dat)) return 0;
        ret = sdc_read_bytes(sdc_base, (uint32_t*)dat->buf, dat->blkcnt, dat->blksz);
    } else if(dat->flag & MMC_DATA_WRITE) {
        if(!sdc_transfer_command(sdc_base, cmd, dat)) return 0;
        ret = sdc_write_bytes(sdc_base, (uint32_t*)dat->buf, dat->blkcnt, dat->blksz);
    }
    return ret;
}

void sdc_reset(uint32_t sdc_base) {
    write32(sdc_base + SDC_GCTL, SDC_HARDWARE_RESET);
}

uint8_t sdc_set_bus_width(uint32_t sdc_base, uint32_t width) {
    switch(width) {
    case MMC_BUS_WIDTH_1:
        write32(sdc_base + SDC_BWDR, SDC_WIDTH1);
        break;
    case MMC_BUS_WIDTH_4:
        write32(sdc_base + SDC_BWDR, SDC_WIDTH4);
        break;
    case MMC_BUS_WIDTH_8:
        write32(sdc_base + SDC_BWDR, SDC_WIDTH8);
        break;
    default:
        return 0;
    }
    return 1;
}

static uint8_t sdc_update_clock(uint32_t sdc_base) {
    uint32_t cmd = (1U << 31) | (1 << 21) | (1 << 13);
    int timeout  = 10000;

    write32(sdc_base + SDC_CMDR, cmd);
    while((read32(sdc_base + SDC_CMDR) & 0x80000000) && timeout--)
        ;
    if(!timeout) return 0;
    write32(sdc_base + SDC_RISR, read32(sdc_base + SDC_RISR));
    return 1;
}

uint8_t sdc_set_clock(uint32_t sdc_base, uint32_t clock) {
    if(sdc_base == SDC0_BASE)
        clk_sdc_config(CCU_SDMMC0_CLK, clock);
    else
        clk_sdc_config(CCU_SDMMC1_CLK, clock);

    write32(sdc_base + SDC_CKCR, 0);
    if(!sdc_update_clock(sdc_base)) return 0;
    write32(sdc_base + SDC_CKCR, read32(sdc_base + SDC_CKCR) | (3 << 16));
    if(!sdc_update_clock(sdc_base)) return 0;
    return 1;
}

uint8_t sdc_transfer(uint32_t sdc_base, sdc_cmd_t* cmd, sdc_data_t* dat) {
    if(dat == NULL) return sdc_transfer_command(sdc_base, cmd, dat);
    return sdc_transfer_data(sdc_base, cmd, dat);
}
