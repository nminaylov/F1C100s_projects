#ifndef __MMC_H__
#define __MMC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t sdc_base;

    uint32_t voltage;
    uint32_t width;
    uint32_t clock;

    uint32_t version;
    uint32_t ocr;
    uint32_t rca;
    uint32_t csd[4];
    uint8_t extcsd[512];

    uint32_t high_capacity;
    uint32_t tran_speed;
    uint32_t read_bl_len;
    uint32_t write_bl_len;
    uint64_t blk_cnt;
    uint64_t capacity;
} sdcard_t;

uint8_t sdcard_detect(sdcard_t *card);
uint64_t sdcard_read(sdcard_t * card, uint8_t * buf, uint64_t blkno, uint64_t blkcnt);
uint64_t sdcard_write(sdcard_t * card, uint8_t * buf, uint64_t blkno, uint64_t blkcnt);


#ifdef __cplusplus
}
#endif

#endif /* __MMC_H__ */
