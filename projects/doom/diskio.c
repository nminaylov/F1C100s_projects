/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/

#include "ff.h" /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "sdcard.h"
#include "f1c100s_gpio.h"
#include "f1c100s_clock.h"
#include "f1c100s_sdc.h"

/* Definitions of physical drive number for each drive */
#define DEV_MMC 0 /* Map MMC/SD card to physical drive 0 */

static sdcard_t sdcard;

DRESULT sdcard_ioctl(BYTE cmd, void* buff);

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(BYTE pdrv) {
    DSTATUS stat;

    switch(pdrv) {
    case DEV_MMC:
        stat = 0;
        return stat;
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(BYTE pdrv) {
    switch(pdrv) {
    case DEV_MMC:
        clk_reset_set(CCU_BUS_SOFT_RST0, 8);
        clk_enable(CCU_BUS_CLK_GATE0, 8);
        clk_reset_clear(CCU_BUS_SOFT_RST0, 8);

        gpio_init(GPIOF, PIN1 | PIN2 | PIN3, GPIO_MODE_AF2, GPIO_PULL_NONE, GPIO_DRV_3);

        sdcard.sdc_base = SDC0_BASE;
        sdcard.voltage  = MMC_VDD_27_36;
        sdcard.width    = MMC_BUS_WIDTH_1;
        sdcard.clock    = 50000000;

        if(sdcard_detect(&sdcard) == 1) return 0;
    }
    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    uint64_t result;

    switch(pdrv) {
    case DEV_MMC:
        result = sdcard_read(&sdcard, buff, sector, count);

        if(result == count)
            return RES_OK;
        else
            return RES_ERROR;
    }
    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    int result;

    switch(pdrv) {
    case DEV_MMC:
        result = sdcard_write(&sdcard, (uint8_t*)buff, sector, count);

        if(result == count)
            return RES_OK;
        else
            return RES_ERROR;
    }
    return RES_PARERR;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
    switch(pdrv) {
    case DEV_MMC:
        return sdcard_ioctl(cmd, buff);
    }
    return RES_PARERR;
}

DRESULT sdcard_ioctl(BYTE cmd, void* buff) {
    switch(cmd) {
    case CTRL_SYNC:
        return RES_OK;
        break;
    case GET_SECTOR_COUNT:
        *(DWORD*)buff = sdcard.blk_cnt;
        return RES_OK;
        break;
    case GET_SECTOR_SIZE:
        *(WORD*)buff = sdcard.read_bl_len;
        return RES_OK;
        break;
    case GET_BLOCK_SIZE:
        *(DWORD*)buff = sdcard.write_bl_len / sdcard.read_bl_len;
        return RES_OK;
        break;
    default:
        break;
    }

    return RES_PARERR;
}
