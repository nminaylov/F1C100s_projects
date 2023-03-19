#include "f1c100s_de.h"
#include <string.h>
#include "f1c100s_clock.h"
#include "f1c100s_tve.h"
#include "io.h"

static void debe_update_linewidth(uint8_t layer);
static void tcon0_init(de_lcd_config_t* params);
static void tcon1_init(tve_mode_e mode);
static void debe_init(void);
static void tcon_deinit(void);
static void tcon_clk_init(void);
static void tcon_clk_enable(void);
static void defe_clk_init(void);
static void defe_clk_enable(void);
static void debe_clk_init(void);
static void debe_clk_enable(void);

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t bits_per_pixel;
} de_layer_params_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    de_layer_params_t layer[4];
    de_mode_e mode;
} de_params_t;

static de_params_t de;

/* TODO:
 *
 *    defe
 *
 *    debe_set_layer_priority
 *
 *    debe_cursor_enable
 *    debe_cursor_disable
 *    debe_cursor_set_pos
 *    debe_cursor_set_size
 *    debe_cursor_write_pattern
 *    debe_cursor_write_palette
 * 
 *     tcon irq
 *
 */
/************** DEBE Layers ***************/

void debe_set_bg_color(uint32_t color) {
    write32(DEBE_BASE + DEBE_BACKCOLOR, color);
}

void debe_layer_enable(uint8_t layer) {
    set32(DEBE_BASE + DEBE_MODE, (1 << (layer + 8)));
}

void debe_layer_disable(uint8_t layer) {
    clear32(DEBE_BASE + DEBE_MODE, (1 << (layer + 8)));
}

void debe_layer_init(uint8_t layer) {
    if(layer > 3) return;
    de.layer[layer].width  = de.width;
    de.layer[layer].height = de.height;

    write32(DEBE_BASE + DEBE_LAY_POS + layer * 4, 0);
    write32(DEBE_BASE + DEBE_LAY_SIZE + layer * 4, ((de.height - 1) << 16) | (de.width - 1));

    debe_update_linewidth(layer);
}

void debe_layer_set_pos(uint8_t layer, int16_t x, int16_t y) {
    if(layer > 3) return;
    write32(DEBE_BASE + DEBE_LAY_POS + layer * 4, (y << 16) | (x & 0xFFFF));
}

void debe_layer_set_size(uint8_t layer, uint16_t w, uint16_t h) {
    if(layer > 3) return;
    de.layer[layer].width  = w;
    de.layer[layer].height = h;

    write32(DEBE_BASE + DEBE_LAY_SIZE + layer * 4, ((h - 1) << 16) | (w - 1));

    debe_update_linewidth(layer);
}

void debe_layer_set_mode(uint8_t layer, debe_color_mode_e mode) {
    if(layer > 3) return;

    if(mode == DEBE_MODE_DEFE_VIDEO) {
        uint32_t val = read32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4) & ~(3 << 1);
        write32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4, val | (1 << 1));
    } else if(mode == DEBE_MODE_YUV) {
        uint32_t val = read32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4) & ~(3 << 1);
        write32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4, val | (1 << 2));
    } else {
        de.layer[layer].bits_per_pixel = (mode >> 8) & 0x00FF;

        if(mode & DEBE_PALETTE_EN) {
            set32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4, (1 << 22));
        } else {
            clear32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4, (1 << 22));
        }

        uint32_t val = read32(DEBE_BASE + DEBE_LAY_ATTR1 + layer * 4) & ~(0x0F << 8);
        write32(DEBE_BASE + DEBE_LAY_ATTR1 + layer * 4, val | ((mode & 0x0F) << 8));

        debe_update_linewidth(layer);
    }
}

// Set framebufer address
void debe_layer_set_addr(uint8_t layer, void* buf) {
    if(layer > 3) return;
    write32(DEBE_BASE + DEBE_LAY_ADDR + layer * 4, ((uint32_t)buf) << 3);
}

void debe_layer_set_alpha(uint8_t layer, uint8_t alpha) {
    if(layer > 3) return;
    uint32_t val = read32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4) & ~(0xFF << 24);
    write32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4, val | (alpha << 24));

    if(alpha != 0) {
        set32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4, (1 << 0));
    } else {
        clear32(DEBE_BASE + DEBE_LAY_ATTR0 + layer * 4, (1 << 0));
    }
}

static void debe_update_linewidth(uint8_t layer) {
    if(layer > 3) return;
    uint32_t val = de.layer[layer].width * de.layer[layer].bits_per_pixel;
    write32(DEBE_BASE + DEBE_LAY_STRIDE + layer * 4, val);
}

void debe_write_palette(uint32_t* data, uint16_t len) {
    memcpy((void*)(DEBE_BASE + DEBE_PALETTE), data, len * 4);
}

void de_lcd_8080_write(uint16_t data, bool is_cmd) {
    while(read32(TCON_BASE + TCON0_CPU_INTF) & 0x00C00000)
        ;

    if(is_cmd) {
        clear32(TCON_BASE + TCON0_CPU_INTF, (1 << 25));
    } else {
        set32(TCON_BASE + TCON0_CPU_INTF, (1 << 25));
    }

    while(read32(TCON_BASE + TCON0_CPU_INTF) & 0x00C00000)
        ;

    uint32_t reg_data = ((data & 0xfc00) << 8) | ((data & 0x0300) << 6) | ((data & 0x00e0) << 5) |
                        ((data & 0x001f) << 3);

    write32(TCON_BASE + TCON0_CPU_WR_DAT, reg_data);
}

void de_lcd_8080_auto_mode(bool enabled) {
    if(enabled) {
        set32(TCON_BASE + TCON0_CPU_INTF, (1 << 28));
    } else {
        clear32(TCON_BASE + TCON0_CPU_INTF, (1 << 28));
    }
}

/************** Initialization ***************/
void de_lcd_init(de_lcd_config_t* params) {
    de.height = params->height;
    de.width  = params->width;
    de.mode   = DE_LCD;

    clk_reset_set(CCU_BUS_SOFT_RST1, 14);
    clk_reset_set(CCU_BUS_SOFT_RST1, 12);
    clk_reset_set(CCU_BUS_SOFT_RST1, 4);

    debe_clk_init();
    defe_clk_init();
    tcon_clk_init();

    defe_clk_enable();
    debe_clk_enable();
    tcon_clk_enable();

    clk_reset_clear(CCU_BUS_SOFT_RST1, 14);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 12);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 4);

    for(uint32_t i = 0x0800; i < 0x1000; i += 4) {
        write32(DEBE_BASE + i, 0);
    }

    tcon_deinit();
    debe_init();
    tcon0_init(params);
    debe_set_bg_color(0);
    de_enable();
    debe_load(DEBE_UPDATE_MANUAL);
}

// clang-format off
static const uint32_t csc_tab[192] = {
    //Y/G   Y/G     Y/G     Y/G     U/R     U/R     U/R     U/R     V/B     V/B     V/B     V/B
    //bt601
    0x04a8, 0x1e70, 0x1cbf, 0x0878, 0x04a8, 0x0000, 0x0662, 0x3211, 0x04a8, 0x0812, 0x0000, 0x2eb1, //yuv2rgb
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //yuv2yuv
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //rgb2rgb
    0x0204, 0x0107, 0x0064, 0x0100, 0x1ed6, 0x1f68, 0x01c2, 0x0800, 0x1e87, 0x01c2, 0x1fb7, 0x0800, //rgb2yuv

    //bt709
    0x04a8, 0x1f26, 0x1ddd, 0x04d0, 0x04a8, 0x0000, 0x072c, 0x307e, 0x04a8, 0x0876, 0x0000, 0x2dea, //yuv2rgb
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //yuv2yuv
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //rgb2rgb
    0x0275, 0x00bb, 0x003f, 0x0100, 0x1ea6, 0x1f99, 0x01c2, 0x0800, 0x1e67, 0x01c2, 0x1fd7, 0x0800, //rgb2yuv

    //DISP_YCC
    0x0400, 0x1e9e, 0x1d24, 0x087b, 0x0400, 0x0000, 0x059c, 0x34c8, 0x0400, 0x0716, 0x0000, 0x31d5, //yuv2rgb
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //yuv2yuv
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //rgb2rgb
    0x0259, 0x0132, 0x0075, 0x0000, 0x1ead, 0x1f53, 0x0200, 0x0800, 0x1e54, 0x0200, 0x1fac, 0x0800, //rgb2yuv

    //xvYCC
    0x04a8, 0x1f26, 0x1ddd, 0x04d0, 0x04a8, 0x0000, 0x072c, 0x307e, 0x04a8, 0x0876, 0x0000, 0x2dea, //yuv2rgb
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //yuv2yuv
    0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, //rgb2rgb
    0x0275, 0x00bb, 0x003f, 0x0100, 0x1ea6, 0x1f99, 0x01c2, 0x0800, 0x1e67, 0x01c2, 0x1fd7, 0x0800, //rgb2yuv
};
// clang-format on

void de_tv_init(tve_mode_e mode, uint16_t hor_lines) {
    de.mode   = DE_TV;
    de.width  = 720;
    de.height = (mode == TVE_MODE_NTSC) ? (480) : (576);

    clk_reset_set(CCU_BUS_SOFT_RST1, 14);
    clk_reset_set(CCU_BUS_SOFT_RST1, 12);
    clk_reset_set(CCU_BUS_SOFT_RST1, 4);

    debe_clk_init();
    defe_clk_init();
    tcon_clk_init();

    defe_clk_enable();
    debe_clk_enable();
    tcon_clk_enable();

    clk_reset_clear(CCU_BUS_SOFT_RST1, 14);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 12);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 4);

    for(uint32_t i = 0x0800; i < 0x1000; i += 4) {
        write32(DEBE_BASE + i, 0);
    }

    tcon_deinit();
    debe_init();
    tcon1_init(mode);

    // CSC configuration
    for(uint8_t i = 0; i < 4; i++) {
        write32(DEBE_BASE + DEBE_COLOR_COEF + i * 4 + 0 * 4, csc_tab[12 * 3 + i] << 16);
        write32(DEBE_BASE + DEBE_COLOR_COEF + i * 4 + 4 * 4, csc_tab[12 * 3 + i + 4] << 16);
        write32(DEBE_BASE + DEBE_COLOR_COEF + i * 4 + 8 * 4, csc_tab[12 * 3 + i + 8] << 16);
    }

    set32(DEBE_BASE + DEBE_MODE, (1 << 5)); // CSC enable

    debe_set_bg_color(0);
    de_enable();
    debe_load(DEBE_UPDATE_MANUAL);

    tve_init(mode);
}

void de_enable(void) {
    if(de.mode == DE_LCD) {
        set32(TCON_BASE + TCON0_CTRL, (1 << 31));
    } else if(de.mode == DE_TV) {
        set32(TCON_BASE + TCON1_CTRL, (1 << 31));
        tve_enable();
    }
    set32(TCON_BASE + TCON_CTRL, (1 << 31));
    set32(DEBE_BASE + DEBE_MODE, (1 << 0));
}

void de_diable(void) {
    if(de.mode == DE_TV) {
        tve_disable();
    }
    clear32(TCON_BASE + TCON_CTRL, (1 << 31));
    clear32(DEBE_BASE + DEBE_MODE, (1 << 0));
}

// Update DEBE registers
void debe_load(debe_reg_update_e mode) {
    write32(DEBE_BASE + DEBE_REGBUF_CTRL, mode);
}

// Initialize DEFE in semi-planar YUV 4:2:2 input mode
void defe_init_spl_422(uint16_t in_w, uint16_t in_h, uint8_t* buf_y, uint8_t* buf_uv) {
    set32(DEFE_BASE + DEFE_EN, 0x01); // Enable DEFE
    set32(DEFE_BASE + DEFE_EN, (1 << 31)); // Enable CPU access

    write32(DEFE_BASE + DEFE_BYPASS, (0 << 0) | (0 << 1)); // CSC/scaler bypass disabled

    write32(DEFE_BASE + DEFE_ADDR0, (uint32_t)buf_y);
    write32(DEFE_BASE + DEFE_ADDR1, (uint32_t)buf_uv);
    write32(DEFE_BASE + DEFE_STRIDE0, in_w);
    write32(DEFE_BASE + DEFE_STRIDE1, in_w);

    write32(DEFE_BASE + DEFE_IN_SIZE, (in_w - 1) | ((in_h - 1) << 16)); // Out size = In size
    write32(DEFE_BASE + DEFE_OUT_SIZE, (in_w - 1) | ((in_h - 1) << 16));
    write32(DEFE_BASE + DEFE_H_FACT, (1 << 16)); // H scale: 1
    if(de.mode == DE_LCD)
        write32(DEFE_BASE + DEFE_V_FACT, (1 << 16)); // V scale: 1
    else if(de.mode == DE_TV)
        write32(DEFE_BASE + DEFE_V_FACT, (2 << 16)); // V scale: 1/2 (??)

    write32(DEFE_BASE + DEFE_IN_FMT, (2 << 8) | (1 << 4)); // UV combined | 422
    set32(DEFE_BASE + DEFE_OUT_FMT, (1 << 4)); //??
    //write32(DEFE_BASE+DEFE_FIELD_CTRL, (1 << 12)); //?

    for(uint8_t i = 0; i < 4; i++) // Color conversion table
    {
        write32(DEFE_BASE + DEFE_CSC_COEF + i * 4 + 0 * 4, csc_tab[i]);
        write32(DEFE_BASE + DEFE_CSC_COEF + i * 4 + 4 * 4, csc_tab[i + 4]);
        write32(DEFE_BASE + DEFE_CSC_COEF + i * 4 + 8 * 4, csc_tab[i + 8]);
    }

    set32(
        DEFE_BASE + DEFE_FRM_CTRL,
        (1 << 23)); // Enable CPU access to filter RAM (if enabled, filter is bypassed?)

    //    for (uint16_t i = 0; i < 32; i++) // TODO:
    //    {
    //        write32(DEFE_BASE+DEFE_CH0_H_COEF+i*4, fir_tab[32*1+i]);
    //        write32(DEFE_BASE+DEFE_CH0_V_COEF+i*4, fir_tab[32*1+i]);
    //        write32(DEFE_BASE+DEFE_CH1_H_COEF+i*4, fir_tab[32*1+i]);
    //        write32(DEFE_BASE+DEFE_CH1_V_COEF+i*4, fir_tab[32*1+i]);
    //    }
    //    clear32(DEFE_BASE+DEFE_FRM_CTRL, (1 << 23)); // Disable CPU access to filter RAM (enable filter?)

    clear32(DEFE_BASE + DEFE_EN, (1 << 31)); // Disable CPU access (?)
    set32(DEFE_BASE + DEFE_FRM_CTRL, (1 << 0)); // Registers ready
    set32(DEFE_BASE + DEFE_FRM_CTRL, (1 << 16)); // Start frame processing
}

// TCON0 -> LCD
static void tcon0_init(de_lcd_config_t* params) {
    int32_t bp, total;
    uint32_t val;

    uint32_t tcon_clk = clk_pll_get_freq(PLL_VIDEO);

    val = (params->v_front_porch + params->v_back_porch + params->v_sync_len);
    write32(TCON_BASE + TCON0_CTRL, ((val & 0x1f) << 4));
    val = tcon_clk / params->pixel_clock_hz;
    write32(TCON_BASE + TCON0_DCLK, (0xf << 28) | (val << 0));
    write32(TCON_BASE + TCON0_TIMING_ACT, ((de.width - 1) << 16) | ((de.height - 1) << 0));

    bp    = params->h_sync_len + params->h_back_porch;
    total = de.width + params->h_front_porch + bp;
    write32(TCON_BASE + TCON0_TIMING_H, ((total - 1) << 16) | ((bp - 1) << 0));

    bp    = params->v_sync_len + params->v_back_porch;
    total = de.height + params->v_front_porch + bp;
    write32(TCON_BASE + TCON0_TIMING_V, ((total * 2) << 16) | ((bp - 1) << 0));
    write32(
        TCON_BASE + TCON0_TIMING_SYNC,
        ((params->h_sync_len - 1) << 16) | ((params->v_sync_len - 1) << 0));

    if(params->bus_mode == DE_LCD_CPU_8080) {
        set32(TCON_BASE + TCON0_CTRL, (1 << 24));
        write32(TCON_BASE + TCON0_HV_INTF, 0);
        write32(TCON_BASE + TCON0_CPU_INTF, (params->bus_8080_type << 29) | (1 << 26));
    } else {
        clear32(TCON_BASE + TCON0_CTRL, (1 << 24));
        if(params->bus_mode == DE_LCD_SERIAL_RGB) { // TODO: RGB order
            write32(TCON_BASE + TCON0_HV_INTF, (1UL << 31));
        } else if(params->bus_mode == DE_LCD_SERIAL_YUV) { // TODO: YUV order
            write32(TCON_BASE + TCON0_HV_INTF, (1UL << 31) | (1UL << 31));
        } else {
            write32(TCON_BASE + TCON0_HV_INTF, 0);
        }
        write32(TCON_BASE + TCON0_CPU_INTF, 0);
    }

    write32(TCON_BASE + TCON_FRM_SEED + 0 * 4, 0x11111111);
    write32(TCON_BASE + TCON_FRM_SEED + 1 * 4, 0x11111111);
    write32(TCON_BASE + TCON_FRM_SEED + 2 * 4, 0x11111111);
    write32(TCON_BASE + TCON_FRM_SEED + 3 * 4, 0x11111111);
    write32(TCON_BASE + TCON_FRM_SEED + 4 * 4, 0x11111111);
    write32(TCON_BASE + TCON_FRM_SEED + 5 * 4, 0x11111111);

    write32(TCON_BASE + TCON_FRM_TABLE + 0 * 4, 0x01010000);
    write32(TCON_BASE + TCON_FRM_TABLE + 1 * 4, 0x15151111);
    write32(TCON_BASE + TCON_FRM_TABLE + 2 * 4, 0x57575555);
    write32(TCON_BASE + TCON_FRM_TABLE + 3 * 4, 0x7f7f7777);

    write32(TCON_BASE + TCON_FRM_CTRL, (params->bus_width << 4) | (1 << 31));

    val = (1 << 28);
    if(params->h_sync_invert) val |= (1 << 25); // io1 ?
    if(params->v_sync_invert) val |= (1 << 24); // io0 ?
    write32(TCON_BASE + TCON0_IO_POLARITY, val);
    write32(TCON_BASE + TCON0_IO_TRISTATE, 0);
}

// TCON1 -> TVE
static void tcon1_init(tve_mode_e mode) {
    if(mode == TVE_MODE_NTSC) {
        write32(TCON_BASE + TCON1_CTRL, 0x00100130);
        write32(TCON_BASE + TCON1_TIMING_SRC, ((720 - 1) << 16) | (480 / 2 - 1));
        write32(TCON_BASE + TCON1_TIMING_SCALE, ((720 - 1) << 16) | (480 / 2 - 1));
        write32(TCON_BASE + TCON1_TIMING_OUT, ((720 - 1) << 16) | (480 / 2 - 1));
        write32(TCON_BASE + TCON1_TIMING_H, ((858 - 1) << 16) | (117));
        write32(TCON_BASE + TCON1_TIMING_V, (525 << 16) | (18));
    } else if(mode == TVE_MODE_PAL) {
        write32(TCON_BASE + TCON1_CTRL, 0x00100150);
        write32(TCON_BASE + TCON1_TIMING_SRC, ((720 - 1) << 16) | (575 / 2 - 1));
        write32(TCON_BASE + TCON1_TIMING_SCALE, ((720 - 1) << 16) | (575 / 2 - 1));
        write32(TCON_BASE + TCON1_TIMING_OUT, ((720 - 1) << 16) | (575 / 2 - 1));
        write32(TCON_BASE + TCON1_TIMING_H, ((864 - 1) << 16) | (138));
        write32(TCON_BASE + TCON1_TIMING_V, (625 << 16) | (22));
    }

    write32(TCON_BASE + TCON1_TIMING_SYNC, 0x00010001);
    write32(TCON_BASE + TCON1_IO_POLARITY, 0x00000000);
    write32(TCON_BASE + TCON1_IO_TRISTATE, 0x0FFFFFFF);
}

static void debe_init(void) {
    write32(DEBE_BASE + DEBE_MODE, (1 << 1));

    for(uint8_t i = 0; i < 4; i++) {
        write32(DEBE_BASE + DEBE_LAY_ATTR0 + i * 4, (i << 10) | ((i & 1) << 15));
        write32(DEBE_BASE + DEBE_LAY_ATTR1 + i * 4, 0);
        de.layer[i].bits_per_pixel = 32;
        debe_layer_init(i);
        debe_layer_set_mode(i, DEBE_MODE_32BPP_RGB_888);
    }

    debe_load(DEBE_UPDATE_MANUAL);
}

static void tcon_deinit(void) {
    write32(DEBE_BASE + TCON_CTRL, 0);
    write32(DEBE_BASE + TCON_INT0, 0);

    write32(DEBE_BASE + TCON0_DCLK, (0xF << 28));

    write32(DEBE_BASE + TCON0_IO_TRISTATE, 0xFFFFFFFF);
    write32(DEBE_BASE + TCON1_IO_TRISTATE, 0xFFFFFFFF);
}

static void tcon_clk_init(void) {
    clk_tcon_config(CLK_VID_SRC_PLL_VIDEO_1X);
}

static void tcon_clk_enable(void) {
    clk_enable(CCU_TCON_CLK, 31);
    clk_enable(CCU_BUS_CLK_GATE1, 4);
}

static void defe_clk_init(void) {
    clk_de_config(CCU_DEFE_CLK, CLK_DE_SRC_PLL_VIDEO, 1);
}

static void defe_clk_enable(void) {
    clk_enable(CCU_DRAM_CLK_GATE, 24);
    clk_enable(CCU_DEFE_CLK, 31);
    clk_enable(CCU_BUS_CLK_GATE1, 14);
}

static void debe_clk_init(void) {
    clk_de_config(CCU_DEBE_CLK, CLK_DE_SRC_PLL_VIDEO, 1);
}

static void debe_clk_enable(void) {
    clk_enable(CCU_DRAM_CLK_GATE, 26);
    clk_enable(CCU_DEBE_CLK, 31);
    clk_enable(CCU_BUS_CLK_GATE1, 12);
}
