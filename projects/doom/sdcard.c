#include <stdint.h>
#include <stddef.h>
#include "f1c100s_sdc.h"
#include "sdcard.h"

static inline void sdelay(int loops);
static int mmc_status(sdcard_t* card);
static uint64_t mmc_read_blocks(sdcard_t* card, uint8_t* buf, uint64_t start, uint64_t blkcnt);
static uint64_t mmc_write_blocks(sdcard_t* card, uint8_t* buf, uint64_t start, uint64_t blkcnt);

static const unsigned tran_speed_unit[] = {10000, 100000, 1000000, 10000000};

static const unsigned char tran_speed_time[] =
    {0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80};

static uint8_t go_idle_state(sdcard_t* card) {
    sdc_cmd_t cmd = {0};

    cmd.cmdidx = MMC_GO_IDLE_STATE;
    cmd.cmdarg = 0;
    cmd.resptype = MMC_RESP_NONE;

    if(sdc_transfer(card->sdc_base, &cmd, NULL)) return 1;
    return sdc_transfer(card->sdc_base, &cmd, NULL);
}

static uint8_t sd_send_if_cond(sdcard_t* card) {
    sdc_cmd_t cmd = {0};

    cmd.cmdidx = MMC_SD_SEND_IF_COND;
    if(card->voltage & MMC_VDD_27_36)
        cmd.cmdarg = (0x1 << 8);
    else if(card->voltage & MMC_VDD_165_195)
        cmd.cmdarg = (0x2 << 8);
    else
        cmd.cmdarg = (0x0 << 8);
    cmd.cmdarg |= 0xaa;
    cmd.resptype = MMC_RESP_R7;
    if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;

    if((cmd.response[0] & 0xff) != 0xaa) return 0;
    card->version = MMC_VERSION_SD_2;
    return 1;
}

static uint8_t sd_send_op_cond(sdcard_t* card) {
    sdc_cmd_t cmd = {0};
    int retries = 100;

    do {
        cmd.cmdidx = MMC_APP_CMD;
        cmd.cmdarg = 0;
        cmd.resptype = MMC_RESP_R1;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) continue;

        cmd.cmdidx = MMC_SD_APP_SEND_OP_COND;
        if(card->voltage & MMC_VDD_27_36)
            cmd.cmdarg = 0x00ff8000;
        else if(card->voltage & MMC_VDD_165_195)
            cmd.cmdarg = 0x00000080;
        else
            cmd.cmdarg = 0;
        if(card->version == MMC_VERSION_SD_2) cmd.cmdarg |= MMC_OCR_HCS;
        cmd.resptype = MMC_RESP_R3;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL) || (cmd.response[0] & MMC_OCR_BUSY)) break;
    } while(retries--);

    if(retries <= 0) return 0;

    if(card->version != MMC_VERSION_SD_2) card->version = MMC_VERSION_SD_1_0;

    card->ocr = cmd.response[0];
    card->high_capacity = ((card->ocr & MMC_OCR_HCS) == MMC_OCR_HCS);
    card->rca = 0;

    return 1;
}

static uint8_t mmc_send_op_cond(sdcard_t* card) {
    sdc_cmd_t cmd = {0};
    int retries = 100;

    if(!go_idle_state(card)) return 0;

    cmd.cmdidx = MMC_SEND_OP_COND;
    cmd.cmdarg = 0;
    cmd.resptype = MMC_RESP_R3;
    if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;

    do {
        cmd.cmdidx = MMC_SEND_OP_COND;
        cmd.cmdarg = (card->ocr & MMC_OCR_VOLTAGE_MASK) | (card->ocr & MMC_OCR_ACCESS_MODE);
        cmd.cmdarg |= MMC_OCR_HCS;
        cmd.resptype = MMC_RESP_R3;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    } while(!(cmd.response[0] & MMC_OCR_BUSY) && retries--);

    if(retries <= 0) return 0;

    card->version = MMC_VERSION_UNKNOWN;
    card->ocr = cmd.response[0];
    card->high_capacity = ((card->ocr & MMC_OCR_HCS) == MMC_OCR_HCS);
    card->rca = 0;
    return 1;
}

static int mmc_status(sdcard_t* card) {
    sdc_cmd_t cmd = {0};
    int retries = 100;

    cmd.cmdidx = MMC_SEND_STATUS;
    cmd.resptype = MMC_RESP_R1;
    cmd.cmdarg = card->rca << 16;
    do {
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) continue;
        if(cmd.response[0] & (1 << 8)) break;
        sdelay(100000); //TODO: delay_ms(1);
    } while(retries-- > 0);
    if(retries > 0) return ((cmd.response[0] >> 9) & 0xf);
    return -1;
}

static uint64_t mmc_read_blocks(sdcard_t* card, uint8_t* buf, uint64_t start, uint64_t blkcnt) {
    sdc_cmd_t cmd = {0};
    sdc_data_t dat = {0};
    int status;

    if(blkcnt > 1)
        cmd.cmdidx = MMC_READ_MULTIPLE_BLOCK;
    else
        cmd.cmdidx = MMC_READ_SINGLE_BLOCK;
    if(card->high_capacity)
        cmd.cmdarg = start;
    else
        cmd.cmdarg = start * card->read_bl_len;
    cmd.resptype = MMC_RESP_R1;
    dat.buf = buf;
    dat.flag = MMC_DATA_READ;
    dat.blksz = card->read_bl_len;
    dat.blkcnt = blkcnt;
    if(!sdc_transfer(card->sdc_base, &cmd, &dat)) return 0;

    do {
        status = mmc_status(card);
        if(status < 0) return 0;
    } while((status != MMC_STATUS_TRAN) && (status != MMC_STATUS_DATA));

    if(blkcnt > 1) {
        cmd.cmdidx = MMC_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resptype = MMC_RESP_R1B;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    }
    return blkcnt;
}

static uint64_t mmc_write_blocks(sdcard_t* card, uint8_t* buf, uint64_t start, uint64_t blkcnt) {
    sdc_cmd_t cmd = {0};
    sdc_data_t dat = {0};
    int status;

    if(blkcnt > 1)
        cmd.cmdidx = MMC_WRITE_MULTIPLE_BLOCK;
    else
        cmd.cmdidx = MMC_WRITE_SINGLE_BLOCK;
    if(card->high_capacity)
        cmd.cmdarg = start;
    else
        cmd.cmdarg = start * card->write_bl_len;
    cmd.resptype = MMC_RESP_R1;
    dat.buf = buf;
    dat.flag = MMC_DATA_WRITE;
    dat.blksz = card->write_bl_len;
    dat.blkcnt = blkcnt;
    if(!sdc_transfer(card->sdc_base, &cmd, &dat)) return 0;
    do {
        status = mmc_status(card);
        if(status < 0) return 0;
    } while((status != MMC_STATUS_TRAN) && (status != MMC_STATUS_RCV));

    if(blkcnt > 1) {
        cmd.cmdidx = MMC_STOP_TRANSMISSION;
        cmd.cmdarg = 0;
        cmd.resptype = MMC_RESP_R1B;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    }
    return blkcnt;
}

uint8_t sdcard_detect(sdcard_t* card) {
    sdc_cmd_t cmd = {0};
    sdc_data_t dat = {0};
    uint64_t csize, cmult;
    uint32_t unit, time;
    int width;
    int status;

    sdc_reset(card->sdc_base);
    sdc_set_clock(card->sdc_base, 400 * 1000);
    sdc_set_bus_width(card->sdc_base, MMC_BUS_WIDTH_1);

    if(!go_idle_state(card)) return 0;

    sd_send_if_cond(card);
    if(!sd_send_op_cond(card)) {
        if(!mmc_send_op_cond(card)) return 0;
    }

    cmd.cmdidx = MMC_ALL_SEND_CID;
    cmd.cmdarg = 0;
    cmd.resptype = MMC_RESP_R2;
    if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    //    card->cid[0] = cmd.response[0];
    //    card->cid[1] = cmd.response[1];
    //    card->cid[2] = cmd.response[2];
    //    card->cid[3] = cmd.response[3];

    cmd.cmdidx = MMC_SD_SEND_RELATIVE_ADDR;
    cmd.cmdarg = card->rca << 16;
    cmd.resptype = MMC_RESP_R6;
    if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    if(card->version & MMC_VERSION_SD) card->rca = (cmd.response[0] >> 16) & 0xffff;

    cmd.cmdidx = MMC_SEND_CSD;
    cmd.cmdarg = card->rca << 16;
    cmd.resptype = MMC_RESP_R2;
    if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    card->csd[0] = cmd.response[0];
    card->csd[1] = cmd.response[1];
    card->csd[2] = cmd.response[2];
    card->csd[3] = cmd.response[3];

    cmd.cmdidx = MMC_SELECT_CARD;
    cmd.cmdarg = card->rca << 16;
    cmd.resptype = MMC_RESP_R1;
    if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    do {
        status = mmc_status(card);
        if(status < 0) return 0;
    } while(status != MMC_STATUS_TRAN);

    if(card->version == MMC_VERSION_UNKNOWN) {
        switch((card->csd[0] >> 26) & 0xf) {
        case 0:
            card->version = MMC_VERSION_1_2;
            break;
        case 1:
            card->version = MMC_VERSION_1_4;
            break;
        case 2:
            card->version = MMC_VERSION_2_2;
            break;
        case 3:
            card->version = MMC_VERSION_3;
            break;
        case 4:
            card->version = MMC_VERSION_4;
            break;
        default:
            card->version = MMC_VERSION_1_2;
            break;
        };
    }

    unit = tran_speed_unit[(card->csd[0] & 0x7)];
    time = tran_speed_time[((card->csd[0] >> 3) & 0xf)];
    card->tran_speed = time * unit;

    card->read_bl_len = 1 << ((card->csd[1] >> 16) & 0xf);

    if(card->version & MMC_VERSION_SD)
        card->write_bl_len = card->read_bl_len;
    else
        card->write_bl_len = 1 << ((card->csd[3] >> 22) & 0xf);
    if(card->read_bl_len > 512) card->read_bl_len = 512;
    if(card->write_bl_len > 512) card->write_bl_len = 512;

    if((card->version & MMC_VERSION_MMC) && (card->version >= MMC_VERSION_4)) {
        cmd.cmdidx = MMC_SEND_EXT_CSD;
        cmd.cmdarg = 0;
        cmd.resptype = MMC_RESP_R1;
        dat.buf = card->extcsd;
        dat.flag = MMC_DATA_READ;
        dat.blksz = 512;
        dat.blkcnt = 1;
        if(!sdc_transfer(card->sdc_base, &cmd, &dat)) return 0;

        do {
            status = mmc_status(card);
            if(status < 0) return 0;
        } while(status != MMC_STATUS_TRAN);

        switch(card->extcsd[192]) {
        case 1:
            card->version = MMC_VERSION_4_1;
            break;
        case 2:
            card->version = MMC_VERSION_4_2;
            break;
        case 3:
            card->version = MMC_VERSION_4_3;
            break;
        case 5:
            card->version = MMC_VERSION_4_41;
            break;
        case 6:
            card->version = MMC_VERSION_4_5;
            break;
        case 7:
            card->version = MMC_VERSION_5_0;
            break;
        case 8:
            card->version = MMC_VERSION_5_1;
            break;
        default:
            break;
        }
    }

    if(card->high_capacity) {
        if(card->version & MMC_VERSION_SD) {
            csize = (card->csd[1] & 0x3f) << 16 | (card->csd[2] & 0xffff0000) >> 16;
            card->blk_cnt = (1 + csize) << 10;
        } else {
            card->blk_cnt = card->extcsd[212] << 0 | card->extcsd[212 + 1] << 8 |
                            card->extcsd[212 + 2] << 16 | card->extcsd[212 + 3] << 24;
        }
    } else {
        csize = (card->csd[1] & 0x3ff) << 2 | (card->csd[2] & 0xc0000000) >> 30;
        cmult = (card->csd[2] & 0x00038000) >> 15;

        card->blk_cnt = (csize + 1) << (cmult + 2);
    }
    card->capacity = card->blk_cnt * card->read_bl_len;

    if(card->version & MMC_VERSION_SD) {
        if((card->width & MMC_BUS_WIDTH_8) || (card->width & MMC_BUS_WIDTH_4))
            width = 2;
        else
            width = 0;

        cmd.cmdidx = MMC_APP_CMD;
        cmd.cmdarg = card->rca << 16;
        cmd.resptype = MMC_RESP_R5;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;

        cmd.cmdidx = MMC_SD_SWITCH_FUNC;
        cmd.cmdarg = width;
        cmd.resptype = MMC_RESP_R1;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;

        if(card->tran_speed < card->clock)
            sdc_set_clock(card->sdc_base, card->tran_speed);
        else
            sdc_set_clock(card->sdc_base, card->clock);
        if((card->width & MMC_BUS_WIDTH_8) || (card->width & MMC_BUS_WIDTH_4))
            sdc_set_bus_width(card->sdc_base, MMC_BUS_WIDTH_4);
        else
            sdc_set_bus_width(card->sdc_base, MMC_BUS_WIDTH_1);
    } else if(card->version & MMC_VERSION_MMC) {
        if(card->width & MMC_BUS_WIDTH_8)
            width = 2;
        else if(card->width & MMC_BUS_WIDTH_4)
            width = 1;
        else
            width = 0;

        cmd.cmdidx = MMC_APP_CMD;
        cmd.cmdarg = card->rca << 16;
        cmd.resptype = MMC_RESP_R5;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;

        cmd.cmdidx = MMC_SD_SWITCH_FUNC;
        cmd.cmdarg = width;
        cmd.resptype = MMC_RESP_R1;
        if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;

        if(card->tran_speed < card->clock)
            sdc_set_clock(card->sdc_base, card->tran_speed);
        else
            sdc_set_clock(card->sdc_base, card->clock);
        if(card->width & MMC_BUS_WIDTH_8)
            sdc_set_bus_width(card->sdc_base, MMC_BUS_WIDTH_8);
        else if(card->width & MMC_BUS_WIDTH_4)
            sdc_set_bus_width(card->sdc_base, MMC_BUS_WIDTH_4);
        else if(card->width & MMC_BUS_WIDTH_1)
            sdc_set_bus_width(card->sdc_base, MMC_BUS_WIDTH_1);
    }

    cmd.cmdidx = MMC_SET_BLOCKLEN;
    cmd.cmdarg = card->read_bl_len;
    cmd.resptype = MMC_RESP_R1;
    if(!sdc_transfer(card->sdc_base, &cmd, NULL)) return 0;
    return 1;
}

uint64_t sdcard_read(sdcard_t* card, uint8_t* buf, uint64_t blkno, uint64_t blkcnt) {
    uint64_t cnt, blks = blkcnt;

    while(blks > 0) {
        cnt = (blks > 127) ? 127 : blks;
        if(mmc_read_blocks(card, buf, blkno, cnt) != cnt) return 0;
        blks -= cnt;
        blkno += cnt;
        buf += cnt * card->read_bl_len;
    }
    return blkcnt;
}

uint64_t sdcard_write(sdcard_t* card, uint8_t* buf, uint64_t blkno, uint64_t blkcnt) {
    uint64_t cnt, blks = blkcnt;

    while(blks > 0) {
        cnt = (blks > 127) ? 127 : blks;
        if(mmc_write_blocks(card, buf, blkno, cnt) != cnt) return 0;
        blks -= cnt;
        blkno += cnt;
        buf += cnt * card->write_bl_len;
    }
    return blkcnt;
}

static inline void sdelay(int loops) {
    __asm__ __volatile__("1:\n"
                         "subs %0, %1, #1\n"
                         "bne 1b"
                         : "=r"(loops)
                         : "0"(loops));
}
