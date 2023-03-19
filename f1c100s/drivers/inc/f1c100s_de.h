#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "f1c100s_tve.h"
#include "f1c100s_periph.h"

typedef enum {
    TCON_CTRL      = 0x00,
    TCON_INT0      = 0x04,
    TCON_INT1      = 0x08,
    TCON_FRM_CTRL  = 0x10,
    TCON_FRM_SEED  = 0x14,
    TCON_FRM_TABLE = 0x2C,

    TCON0_CTRL        = 0x40,
    TCON0_DCLK        = 0x44,
    TCON0_TIMING_ACT  = 0x48,
    TCON0_TIMING_H    = 0x4C,
    TCON0_TIMING_V    = 0x50,
    TCON0_TIMING_SYNC = 0x54,
    TCON0_HV_INTF     = 0x58,
    TCON0_CPU_INTF    = 0x60,
    TCON0_CPU_WR_DAT  = 0x64,
    TCON0_CPU_RD_DAT0 = 0x68,
    TCON0_CPU_RD_DAT1 = 0x6C,
    TCON0_IO_POLARITY = 0x88,
    TCON0_IO_TRISTATE = 0x8C,

    TCON1_CTRL         = 0x90,
    TCON1_TIMING_SRC   = 0x94,
    TCON1_TIMING_SCALE = 0x98,
    TCON1_TIMING_OUT   = 0x9C,
    TCON1_TIMING_H     = 0xA0,
    TCON1_TIMING_V     = 0xA4,
    TCON1_TIMING_SYNC  = 0xA8,
    TCON1_IO_POLARITY  = 0xF0,
    TCON1_IO_TRISTATE  = 0xF4,

    TCON_DEBUG = 0xFC,
} tcon_reg_e;

typedef enum {
    DEBE_MODE        = 0x0800,
    DEBE_BACKCOLOR   = 0x0804,
    DEBE_LAY_SIZE    = 0x0810,
    DEBE_LAY_POS     = 0x0820,
    DEBE_LAY_STRIDE  = 0x0840,
    DEBE_LAY_ADDR    = 0x0850,
    DEBE_REGBUF_CTRL = 0x0870,
    DEBE_CKEY_MAX    = 0x0880,
    DEBE_CKEY_MIN    = 0x0884,
    DEBE_CKEY_CFG    = 0x0888,
    DEBE_LAY_ATTR0   = 0x0890,
    DEBE_LAY_ATTR1   = 0x08A0,
    DEBE_HWC_CTRL    = 0x08D8,
    DEBE_HWC_FORMAT  = 0x08E0,
    DEBE_WB_CTRL     = 0x08F0,
    DEBE_WB_ADDR     = 0x08F4,
    DEBE_WB_STRIDE   = 0x08F8,
    DEBE_YUV_IN_CTRL = 0x0920,
    DEBE_YUV_ADDR    = 0x0930,
    DEBE_YUV_STRIDE  = 0x0940,
    DEBE_COLOR_COEF  = 0x0950,
    DEBE_PALETTE     = 0x1000,
    DEBE_HWC_PATTERN = 0x1400,
    DEBE_HWC_PALETTE = 0x1600,
} debe_reg_e;

typedef enum {
    DEFE_EN         = 0x000,
    DEFE_FRM_CTRL   = 0x004,
    DEFE_BYPASS     = 0x008,
    DEFE_AGTH_SEL   = 0x00C,
    DEFE_INT_LINE   = 0x010,
    DEFE_ADDR0      = 0x020,
    DEFE_ADDR1      = 0x024,
    DEFE_ADDR2      = 0x028,
    DEFE_FIELD_CTRL = 0x02C,
    DEFE_TB_OFF0    = 0x030,
    DEFE_TB_OFF1    = 0x034,
    DEFE_TB_OFF2    = 0x038,
    DEFE_STRIDE0    = 0x040,
    DEFE_STRIDE1    = 0x044,
    DEFE_STRIDE2    = 0x048,
    DEFE_IN_FMT     = 0x04C,
    DEFE_WB_ADDR    = 0x050,
    DEFE_OUT_FMT    = 0x05C,
    DEFE_INT_EN     = 0x060,
    DEFE_INT_STATUS = 0x064,
    DEFE_STATUS     = 0x068,
    DEFE_CSC_COEF   = 0x070,
    DEFE_IN_SIZE    = 0x100,
    DEFE_OUT_SIZE   = 0x104,
    DEFE_H_FACT     = 0x108,
    DEFE_V_FACT     = 0x10C,
    DEFE_CH0_H_COEF = 0x400,
    DEFE_CH0_V_COEF = 0x500,
    DEFE_CH1_H_COEF = 0x600,
    DEFE_CH1_V_COEF = 0x700,
} defe_reg_e;

typedef enum {
    DE_LCD_R_5BITS = (1 << 2),
    DE_LCD_R_6BITS = (0 << 2),
    DE_LCD_G_5BITS = (1 << 1),
    DE_LCD_G_6BITS = (0 << 1),
    DE_LCD_B_5BITS = (1 << 0),
    DE_LCD_B_6BITS = (0 << 0),
} de_lcd_bus_e;

typedef enum {
    DE_LCD_PARALLEL_RGB,
    DE_LCD_SERIAL_RGB,
    DE_LCD_SERIAL_YUV,
    DE_LCD_CPU_8080,
} de_lcd_bus_mode_e;

typedef enum {
    DE_8080_MODE_18BIT_256K = 0,
    DE_8080_MODE_16BIT_0    = 1,
    DE_8080_MODE_16BIT_1    = 2,
    DE_8080_MODE_16BIT_2    = 3,
    DE_8080_MODE_16BIT_3    = 4,
    DE_8080_MODE_9BIT       = 5,
    DE_8080_MODE_8BIT_256K  = 6,
    DE_8080_MODE_8BIT_65K   = 7,
} de_lcd_8080_bus_e;

typedef enum {
    DE_LCD,
    DE_TV,
} de_mode_e;

typedef enum {
    DEBE_UPDATE_MANUAL = 3,
    DEBE_UPDATE_AUTO   = 0,
} debe_reg_update_e;

typedef enum {
    DEBE_1BPP  = (1 << 8),
    DEBE_2BPP  = (2 << 8),
    DEBE_4BPP  = (4 << 8),
    DEBE_8BPP  = (8 << 8),
    DEBE_16BPP = (16 << 8),
    DEBE_24BPP = (24 << 8),
    DEBE_32BPP = (32 << 8),
} debe_color_mode_bpp_e;

#define DEBE_PALETTE_EN 0x80

typedef enum {
    DEBE_MODE_1BPP_MONO       = 0 | DEBE_1BPP,
    DEBE_MODE_2BPP_MONO       = 1 | DEBE_2BPP,
    DEBE_MODE_4BPP_MONO       = 2 | DEBE_4BPP,
    DEBE_MODE_8BPP_MONO       = 3 | DEBE_8BPP,
    DEBE_MODE_16BPP_RGB_655   = 4 | DEBE_16BPP,
    DEBE_MODE_16BPP_RGB_565   = 5 | DEBE_16BPP,
    DEBE_MODE_16BPP_RGB_556   = 6 | DEBE_16BPP,
    DEBE_MODE_16BPP_ARGB_1555 = 7 | DEBE_16BPP,
    DEBE_MODE_16BPP_RGBA_5551 = 8 | DEBE_16BPP,
    DEBE_MODE_32BPP_RGB_888   = 9 | DEBE_32BPP,
    DEBE_MODE_32BPP_ARGB_8888 = 10 | DEBE_32BPP,
    DEBE_MODE_24BPP_RGB_888   = 11 | DEBE_24BPP,
    DEBE_MODE_1BPP_PALETTE    = 0 | DEBE_PALETTE_EN | DEBE_1BPP,
    DEBE_MODE_2BPP_PALETTE    = 1 | DEBE_PALETTE_EN | DEBE_2BPP,
    DEBE_MODE_4BPP_PALETTE    = 2 | DEBE_PALETTE_EN | DEBE_4BPP,
    DEBE_MODE_8BPP_PALETTE    = 3 | DEBE_PALETTE_EN | DEBE_8BPP,
    DEBE_MODE_DEFE_VIDEO      = 0x40,
    DEBE_MODE_YUV             = 0x41,
} debe_color_mode_e;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t bus_width;
    uint32_t bus_mode;
    uint32_t bus_8080_type;

    uint32_t pixel_clock_hz;
    uint32_t h_front_porch;
    uint32_t h_back_porch;
    uint32_t h_sync_len;
    uint32_t v_front_porch;
    uint32_t v_back_porch;
    uint32_t v_sync_len;
    uint32_t h_sync_invert;
    uint32_t v_sync_invert;
} de_lcd_config_t;

void debe_set_bg_color(uint32_t color);

void debe_layer_enable(uint8_t layer);

void debe_layer_disable(uint8_t layer);

void debe_layer_init(uint8_t layer);

void debe_layer_set_pos(uint8_t layer, int16_t x, int16_t y);

void debe_layer_set_size(uint8_t layer, uint16_t w, uint16_t h);

void debe_layer_set_mode(uint8_t layer, debe_color_mode_e mode);

void debe_layer_set_addr(uint8_t layer, void* buf);

void debe_layer_set_alpha(uint8_t layer, uint8_t alpha);

void debe_write_palette(uint32_t* data, uint16_t len);

void debe_load(debe_reg_update_e mode);

void defe_init_spl_422(uint16_t in_w, uint16_t in_h, uint8_t* buf_y, uint8_t* buf_uv);

void de_lcd_init(de_lcd_config_t* params);

void de_lcd_8080_write(uint16_t data, bool is_cmd);

void de_lcd_8080_auto_mode(bool enabled);

void de_tv_init(tve_mode_e mode, uint16_t hor_lines);

void de_enable(void);

void de_diable(void);

#ifdef __cplusplus
}
#endif
