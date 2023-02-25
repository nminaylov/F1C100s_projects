#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    SDC_GCTL  = 0x000,
    SDC_CKCR  = 0x004,
    SDC_TMOR  = 0x008,
    SDC_BWDR  = 0x00C,
    SDC_BKSR  = 0x010,
    SDC_BYCR  = 0x014,
    SDC_CMDR  = 0x018,
    SDC_CAGR  = 0x01C,
    SDC_RESP0 = 0x020,
    SDC_RESP1 = 0x024,
    SDC_RESP2 = 0x028,
    SDC_RESP3 = 0x02C,
    SDC_IMKR  = 0x030,
    SDC_MISR  = 0x034,
    SDC_RISR  = 0x038,
    SDC_STAR  = 0x03C,
    SDC_FWLR  = 0x040,
    SDC_FUNS  = 0x044,
    SDC_CBCR  = 0x048,
    SDC_BBCR  = 0x04C,
    SDC_DBCR  = 0x050,
    SDC_A12A  = 0x058,

    SDC_HWRST = 0x078,
    SDC_DMAC  = 0x080,
    SDC_DLBA  = 0x084,
    SDC_IDST  = 0x088,
    SDC_IDIE  = 0x08C,
    SDC_THLDC = 0x100,
    SDC_DSBD  = 0x10C,

    SDC_FIFO = 0x200,
} sdc_reg_e;

typedef struct {
    uint32_t cmdidx;
    uint32_t cmdarg;
    uint32_t resptype;
    uint32_t response[4];
} sdc_cmd_t;

typedef struct {
    uint8_t* buf;
    uint32_t flag;
    uint32_t blksz;
    uint32_t blkcnt;
} sdc_data_t;

/*
 * Global control register bits
 */
typedef enum {
    SDC_SOFT_RESET           = (1 << 0),
    SDC_FIFO_RESET           = (1 << 1),
    SDC_DMA_RESET            = (1 << 2),
    SDC_INTERRUPT_ENABLE_BIT = (1 << 4),
    SDC_DMA_ENABLE_BIT       = (1 << 5),
    SDC_DEBOUNCE_ENABLE_BIT  = (1 << 8),
    SDC_POSEDGE_LATCH_DATA   = (1 << 9),
    SDC_DDR_MODE             = (1 << 10),
    SDC_MEMORY_ACCESS_DONE   = (1 << 29),
    SDC_ACCESS_DONE_DIRECT   = (1 << 30),
    SDC_ACCESS_BY_AHB        = (1 << 31),
    SDC_ACCESS_BY_DMA        = (0 << 31),
    SDC_HARDWARE_RESET       = (SDC_SOFT_RESET | SDC_FIFO_RESET | SDC_DMA_RESET),
} sdc_gctl_bits_e;

/*
 * Clock control bits
 */
#define SDC_CARD_CLOCK_ON (1 << 16)
#define SDC_LOW_POWER_ON (1 << 17)

/*
 * Bus width
 */
#define SDC_WIDTH1 (0)
#define SDC_WIDTH4 (1)
#define SDC_WIDTH8 (2)

/*
 * Smc command bits
 */
#define SDC_RESP_EXPIRE (1 << 6)
#define SDC_LONG_RESPONSE (1 << 7)
#define SDC_CHECK_RESPONSE_CRC (1 << 8)
#define SDC_DATA_EXPIRE (1 << 9)
#define SDC_WRITE (1 << 10)
#define SDC_SEQUENCE_MODE (1 << 11)
#define SDC_SEND_AUTO_STOP (1 << 12)
#define SDC_WAIT_PRE_OVER (1 << 13)
#define SDC_STOP_ABORT_CMD (1 << 14)
#define SDC_SEND_INIT_SEQUENCE (1 << 15)
#define SDC_UPCLK_ONLY (1 << 21)
#define SDC_READ_CEATA_DEV (1 << 22)
#define SDC_CCS_EXPIRE (1 << 23)
#define SDC_ENABLE_BIT_BOOT (1 << 24)
#define SDC_ALT_BOOT_OPTIONS (1 << 25)
#define SDC_BOOT_ACK_EXPIRE (1 << 26)
#define SDC_BOOT_ABORT (1 << 27)
#define SDC_VOLTAGE_SWITCH (1 << 28)
#define SDC_USE_HOLD_REGISTER (1 << 29)
#define SDC_START (1 << 31)

/*
 * Interrupt bits
 */
#define SDC_RESP_ERROR (1 << 1)
#define SDC_COMMAND_DONE (1 << 2)
#define SDC_DATA_OVER (1 << 3)
#define SDC_TX_DATA_REQUEST (1 << 4)
#define SDC_RX_DATA_REQUEST (1 << 5)
#define SDC_RESP_CRC_ERROR (1 << 6)
#define SDC_DATA_CRC_ERROR (1 << 7)
#define SDC_RESP_TIMEOUT (1 << 8)
#define SDC_DATA_TIMEOUT (1 << 9)
#define SDC_VOLTAGE_CHANGE_DONE (1 << 10)
#define SDC_FIFO_RUN_ERROR (1 << 11)
#define SDC_HARD_WARE_LOCKED (1 << 12)
#define SDC_START_BIT_ERROR (1 << 13)
#define SDC_AUTO_COMMAND_DONE (1 << 14)
#define SDC_END_BIT_ERROR (1 << 15)
#define SDC_SDIO_INTERRUPT (1 << 16)
#define SDC_CARD_INSERT (1 << 30)
#define SDC_CARD_REMOVE (1 << 31)
#define SDC_INTERRUPT_ERROR_BIT                                                           \
    (SDC_RESP_ERROR | SDC_RESP_CRC_ERROR | SDC_DATA_CRC_ERROR | SDC_RESP_TIMEOUT |        \
     SDC_DATA_TIMEOUT | SDC_FIFO_RUN_ERROR | SDC_HARD_WARE_LOCKED | SDC_START_BIT_ERROR | \
     SDC_END_BIT_ERROR)
#define SDC_INTERRUPT_DONE_BIT \
    (SDC_AUTO_COMMAND_DONE | SDC_DATA_OVER | SDC_COMMAND_DONE | SDC_VOLTAGE_CHANGE_DONE)

/*
 * Status
 */
#define SDC_RXWL_FLAG (1 << 0)
#define SDC_TXWL_FLAG (1 << 1)
#define SDC_FIFO_EMPTY (1 << 2)
#define SDC_FIFO_FULL (1 << 3)
#define SDC_CARD_PRESENT (1 << 8)
#define SDC_CARD_DATA_BUSY (1 << 9)
#define SDC_DATA_FSM_BUSY (1 << 10)
#define SDC_DMA_REQUEST (1 << 31)
#define SDC_FIFO_SIZE (16)

/*
 * Function select
 */
#define SDC_CEATA_ON (0xCEAA << 16)
#define SDC_SEND_IRQ_RESPONSE (1 << 0)
#define SDC_SDIO_READ_WAIT (1 << 1)
#define SDC_ABORT_READ_DATA (1 << 2)
#define SDC_SEND_CCSD (1 << 8)
#define SDC_SEND_AUTO_STOPCCSD (1 << 9)
#define SDC_CEATA_DEV_IRQ_ENABLE (1 << 10)

/*
 * MMC/SD card defines
 */
typedef enum {
    /* Class 1 */
    MMC_GO_IDLE_STATE       = 0,
    MMC_SEND_OP_COND        = 1,
    MMC_ALL_SEND_CID        = 2,
    MMC_SET_RELATIVE_ADDR   = 3,
    MMC_SET_DSR             = 4,
    MMC_SWITCH              = 6,
    MMC_SELECT_CARD         = 7,
    MMC_SEND_EXT_CSD        = 8,
    MMC_SEND_CSD            = 9,
    MMC_SEND_CID            = 10,
    MMC_READ_DAT_UNTIL_STOP = 11,
    MMC_STOP_TRANSMISSION   = 12,
    MMC_SEND_STATUS         = 13,
    MMC_GO_INACTIVE_STATE   = 15,
    MMC_SPI_READ_OCR        = 58,
    MMC_SPI_CRC_ON_OFF      = 59,

    /* Class 2 */
    MMC_SET_BLOCKLEN        = 16,
    MMC_READ_SINGLE_BLOCK   = 17,
    MMC_READ_MULTIPLE_BLOCK = 18,

    /* Class 3 */
    MMC_WRITE_DAT_UNTIL_STOP = 20,

    /* Class 4 */
    MMC_SET_BLOCK_COUNT      = 23,
    MMC_WRITE_SINGLE_BLOCK   = 24,
    MMC_WRITE_MULTIPLE_BLOCK = 25,
    MMC_PROGRAM_CID          = 26,
    MMC_PROGRAM_CSD          = 27,

    /* Class 5 */
    MMC_ERASE_GROUP_START = 35,
    MMC_ERASE_GROUP_END   = 36,
    MMC_ERASE             = 38,

    /* Class 6 */
    MMC_SET_WRITE_PROT  = 28,
    MMC_CLR_WRITE_PROT  = 29,
    MMC_SEND_WRITE_PROT = 30,

    /* Class 7 */
    MMC_LOCK_UNLOCK = 42,

    /* Class 8 */
    MMC_APP_CMD = 55,
    MMC_GEN_CMD = 56,

    /* Class 9 */
    MMC_FAST_IO      = 39,
    MMC_GO_IRQ_STATE = 40,

    /* SD Commands */
    MMC_SD_SEND_RELATIVE_ADDR = 3,
    MMC_SD_SWITCH_FUNC        = 6,
    MMC_SD_SEND_IF_COND       = 8,
    MMC_SD_APP_SET_BUS_WIDTH  = 6,
    MMC_SD_ERASE_WR_BLK_START = 32,
    MMC_SD_ERASE_WR_BLK_END   = 33,
    MMC_SD_APP_SEND_OP_COND   = 41,
    MMC_SD_APP_SEND_SCR       = 51,
} mmc_cmd_e;

typedef enum {
    MMC_RESP_PRESENT = (1 << 0),
    MMC_RESP_136     = (1 << 1),
    MMC_RESP_CRC     = (1 << 2),
    MMC_RESP_BUSY    = (1 << 3),
    MMC_RESP_OPCODE  = (1 << 4),
} mmc_resp_flags_e;

typedef enum {
    MMC_RESP_NONE = (0 << 24),
    MMC_RESP_R1   = (1 << 24) | (MMC_RESP_PRESENT | MMC_RESP_CRC | MMC_RESP_OPCODE),
    MMC_RESP_R1B = (1 << 24) | (MMC_RESP_PRESENT | MMC_RESP_CRC | MMC_RESP_OPCODE | MMC_RESP_BUSY),
    MMC_RESP_R2  = (2 << 24) | (MMC_RESP_PRESENT | MMC_RESP_136 | MMC_RESP_CRC),
    MMC_RESP_R3  = (3 << 24) | (MMC_RESP_PRESENT),
    MMC_RESP_R4  = (4 << 24) | (MMC_RESP_PRESENT),
    MMC_RESP_R5  = (5 << 24) | (MMC_RESP_PRESENT | MMC_RESP_CRC | MMC_RESP_OPCODE),
    MMC_RESP_R6  = (6 << 24) | (MMC_RESP_PRESENT | MMC_RESP_CRC | MMC_RESP_OPCODE),
    MMC_RESP_R7  = (7 << 24) | (MMC_RESP_PRESENT | MMC_RESP_CRC | MMC_RESP_OPCODE),
} mmc_resp_type_e;

typedef enum {
    MMC_STATUS_IDLE  = 0,
    MMC_STATUS_READY = 1,
    MMC_STATUS_IDENT = 2,
    MMC_STATUS_STBY  = 3,
    MMC_STATUS_TRAN  = 4,
    MMC_STATUS_DATA  = 5,
    MMC_STATUS_RCV   = 6,
    MMC_STATUS_PRG   = 7,
    MMC_STATUS_DIS   = 8,
    MMC_STATUS_BTST  = 9,
    MMC_STATUS_SLP   = 10,
} mmc_status_e;

typedef enum {
    MMC_OCR_BUSY         = 0x80000000,
    MMC_OCR_HCS          = 0x40000000,
    MMC_OCR_VOLTAGE_MASK = 0x00ffff80,
    MMC_OCR_ACCESS_MODE  = 0x60000000,
} mmc_ocr_mask_e;

typedef enum {
    MMC_DATA_READ  = (1 << 0),
    MMC_DATA_WRITE = (1 << 1),
} mmc_act_e;

typedef enum {
    MMC_VDD_27_36   = (1 << 0),
    MMC_VDD_165_195 = (1 << 1),
} mmc_vdd_val_e;

typedef enum {
    MMC_BUS_WIDTH_1 = (1 << 0),
    MMC_BUS_WIDTH_4 = (1 << 2),
    MMC_BUS_WIDTH_8 = (1 << 3),
} mmc_bus_width_e;

typedef enum {
    MMC_VERSION_SD      = 0x20000,
    MMC_VERSION_SD_3    = (MMC_VERSION_SD | 0x300),
    MMC_VERSION_SD_2    = (MMC_VERSION_SD | 0x200),
    MMC_VERSION_SD_1_0  = (MMC_VERSION_SD | 0x100),
    MMC_VERSION_SD_1_10 = (MMC_VERSION_SD | 0x10a),
    MMC_VERSION_MMC     = 0x10000,
    MMC_VERSION_UNKNOWN = (MMC_VERSION_MMC),
    MMC_VERSION_1_2     = (MMC_VERSION_MMC | 0x102),
    MMC_VERSION_1_4     = (MMC_VERSION_MMC | 0x104),
    MMC_VERSION_2_2     = (MMC_VERSION_MMC | 0x202),
    MMC_VERSION_3       = (MMC_VERSION_MMC | 0x300),
    MMC_VERSION_4       = (MMC_VERSION_MMC | 0x400),
    MMC_VERSION_4_1     = (MMC_VERSION_MMC | 0x401),
    MMC_VERSION_4_2     = (MMC_VERSION_MMC | 0x402),
    MMC_VERSION_4_3     = (MMC_VERSION_MMC | 0x403),
    MMC_VERSION_4_41    = (MMC_VERSION_MMC | 0x429),
    MMC_VERSION_4_5     = (MMC_VERSION_MMC | 0x405),
    MMC_VERSION_5_0     = (MMC_VERSION_MMC | 0x500),
    MMC_VERSION_5_1     = (MMC_VERSION_MMC | 0x501),
} mmc_version_e;

void sdc_reset(uint32_t sdc_base);

uint8_t sdc_set_bus_width(uint32_t sdc_base, uint32_t width);

uint8_t sdc_set_clock(uint32_t sdc_base, uint32_t clock);

uint8_t sdc_transfer(uint32_t sdc_base, sdc_cmd_t* cmd, sdc_data_t* dat);

#ifdef __cplusplus
}
#endif
