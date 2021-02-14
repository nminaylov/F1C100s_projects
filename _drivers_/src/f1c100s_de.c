/********************************************//**
 * @file f1c100s_de.c
 * @brief F1C100s Display Engine driver (tcon, debe, defe, tve)
 * @author n.minaylov
 ***********************************************/

#include "f1c100s_de.h"
#include <string.h>
#include "f1c100s_clock.h"
#include "io.h"

static void debe_update_linewidth(uint8_t layer);
static void tcon_init(de_config_t *params, uint32_t tcon_clk);
static void debe_init(void);
static void tcon_deinit(void);
static void tcon_clk_init(void);
static void tcon_clk_enable(void);
//static void tcon_clk_disable(void);
static void defe_clk_init(void);
static void defe_clk_enable(void);
//static void defe_clk_disable(void);
static void debe_clk_init(void);
static void debe_clk_enable(void);
//static void debe_clk_disable(void);

typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t bits_per_pixel;
} de_layer_params_t;

typedef struct
{
    int32_t width;
    int32_t height;
    de_layer_params_t layer[4];
} de_params_t;

static de_params_t de;

struct f1c100s_tcon_reg_t *tcon = (struct f1c100s_tcon_reg_t*) F1C100S_TCON_BASE;
struct f1c100s_debe_reg_t *debe = (struct f1c100s_debe_reg_t*) F1C100S_DEBE_BASE;
struct f1c100s_defe_reg_t *defe = (struct f1c100s_defe_reg_t*) F1C100S_DEFE_BASE;

/* TODO:
 *
 *    defe
 *    tve + tcon1
 *
 *    debe_set_layer_priority
 *     debe_set_yuv_layer
 *     debe_set_video_layer
 *
 *     debe_cursor_enable
 *    debe_cursor_disable
 *    debe_cursor_set_pos
 *    debe_cursor_set_size
 *    debe_cursor_write_pattern
 *    debe_cursor_write_palette
 *
 */
/************** DEBE Layers ***************/
void debe_set_bg_color(uint32_t color) // RGB888
{
    debe->backcolor = color;
}

void debe_layer_enable(uint8_t layer)
{
    debe->mode |= (1 << (layer + 8));
}

void debe_layer_disable(uint8_t layer)
{
    debe->mode &= ~(1 << (layer + 8));
}

void debe_layer_init(uint8_t layer)
{
    if (layer > 3)
        return;

    de.layer[layer].width = de.width;
    de.layer[layer].height = de.height;

    debe->layer_pos[layer] = 0;
    debe->layer_size[layer] = ((de.height - 1) << 16) | (de.width - 1);

    debe_update_linewidth(layer);
}

void debe_layer_set_pos(uint8_t layer, int16_t x, int16_t y)
{
    if (layer > 3)
        return;
    debe->layer_pos[layer] = (y << 16) | (x & 0xFFFF);
}

void debe_layer_set_size(uint8_t layer, uint16_t w, uint16_t h)
{
    if (layer > 3)
        return;
    de.layer[layer].width = w;
    de.layer[layer].height = h;

    debe->layer_size[layer] = ((h - 1) << 16) | (w - 1);

    debe_update_linewidth(layer);
}

void debe_layer_set_mode(uint8_t layer, debe_color_mode_e mode)
{
    if (layer > 3)
        return;
    de.layer[layer].bits_per_pixel = (mode >> 8) & 0x00FF;

    if (mode & DEBE_PALETTE_EN)
        debe->layer_attr0_ctrl[layer] |= (1 << 22);
    else
        debe->layer_attr0_ctrl[layer] &= ~(1 << 22);

    debe->layer_attr1_ctrl[layer] &= ~(0x0F << 8);
    debe->layer_attr1_ctrl[layer] |= (mode & 0x000F) << 8;

    debe_update_linewidth(layer);
}

void debe_layer_set_addr(uint8_t layer, void *buf) // Set framebufer address
{
    if (layer > 3)
        return;

    debe->layer_addr[layer] = ((uint32_t) buf) << 3;
}

void debe_layer_set_alpha(uint8_t layer, uint8_t alpha)
{
    if (layer > 3)
        return;

    debe->layer_attr0_ctrl[layer] &= ~(0xFF << 24);
    debe->layer_attr0_ctrl[layer] |= (alpha << 24);
    if (alpha != 0)
    {
        debe->layer_attr0_ctrl[layer] |= (1 << 0);
    }
    else
    {
        debe->layer_attr0_ctrl[layer] &= ~(1 << 0);
    }
}

static void debe_update_linewidth(uint8_t layer)
{
    if (layer > 3)
        return;

    debe->layer_stride[layer] = de.layer[layer].width * de.layer[layer].bits_per_pixel;
}

void debe_write_palette(uint32_t *data, uint16_t len)
{
    memcpy((void*) (F1C100S_DEBE_BASE + 0x1000), data, len * 4);
}

/************** Initialization ***************/
void de_init(de_config_t *params)
{
    de.height = params->height;
    de.width = params->width;

    clk_pll_init(PLL_VIDEO, 99, 8); // 24*99/8 = 297MHz
    uint32_t tcon_clk = (24 * 99 / 8) * 1000000;
    clk_pll_enable(PLL_VIDEO);

    debe_clk_init();
    defe_clk_init();
    tcon_clk_init();

    defe_clk_enable();
    debe_clk_enable();
    tcon_clk_enable();

    clk_reset_clear(CCU_BUS_SOFT_RST1, 14);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 12);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 4);

    for (uint32_t i = 0x0800; i < 0x1000; i += 4)
        write32(F1C100S_DEBE_BASE + i, 0);

    tcon_deinit();
    debe_init();
    tcon_init(params, tcon_clk);
    debe_set_bg_color(0);
    de_enable();
    debe_load(DEBE_UPDATE_MANUAL);
}

void de_enable(void)
{
    tcon->tcon0_ctrl |= (1 << 31);
    tcon->ctrl |= (1 << 31);
    debe->mode |= (1 << 0);
}

void de_diable(void)
{
    //tcon->tcon0_ctrl &= ~(1 << 31);
    debe->mode &= ~(1 << 0);
}

// Update DEBE registers
void debe_load(debe_reg_update_e mode)
{
    debe->reg_ctrl = mode;
}

static void tcon_init(de_config_t *params, uint32_t tcon_clk)
{
    int32_t bp, total;
    uint32_t val;

    //tcon->ctrl = (1 << 31);
    tcon->ctrl &= ~(0x1 << 0);

    val = (params->v_front_porch + params->v_back_porch + params->v_sync_len);
    tcon->tcon0_ctrl = ((val & 0x1f) << 4);
    val = tcon_clk / params->pixel_clock_hz;
    tcon->tcon0_dclk = (0xf << 28) | (val << 0);
    tcon->tcon0_timing_active = ((de.width - 1) << 16) | ((de.height - 1) << 0);

    bp = params->h_sync_len + params->h_back_porch;
    total = de.width + params->h_front_porch + bp;
    tcon->tcon0_timing_h = ((total - 1) << 16) | ((bp - 1) << 0);
    bp = params->v_sync_len + params->v_back_porch;
    total = de.height + params->v_front_porch + bp;
    tcon->tcon0_timing_v = ((total * 2) << 16) | ((bp - 1) << 0);
    tcon->tcon0_timing_sync = ((params->h_sync_len - 1) << 16) | ((params->v_sync_len - 1) << 0);

    tcon->tcon0_hv_intf = 0;
    tcon->tcon0_cpu_intf = 0;

    tcon->tcon0_frm_seed[0] = 0x11111111;
    tcon->tcon0_frm_seed[1] = 0x11111111;
    tcon->tcon0_frm_seed[2] = 0x11111111;
    tcon->tcon0_frm_seed[3] = 0x11111111;
    tcon->tcon0_frm_seed[4] = 0x11111111;
    tcon->tcon0_frm_seed[5] = 0x11111111;
    tcon->tcon0_frm_table[0] = 0x01010000;
    tcon->tcon0_frm_table[1] = 0x15151111;
    tcon->tcon0_frm_table[2] = 0x57575555;
    tcon->tcon0_frm_table[3] = 0x7f7f7777;
    tcon->tcon0_frm_ctrl = (params->bus_width << 4) | (1 << 31);

    val = (1 << 28);
    if (params->h_sync_invert)
        val |= (1 << 25); // io1
    if (params->h_sync_invert)
        val |= (1 << 24); // io0
    tcon->tcon0_io_polarity = val;
    tcon->tcon0_io_tristate = 0;
}

static void debe_init(void)
{
    debe->disp_size = (((de.height) - 1) << 16) | (((de.width) - 1) << 0);

    debe->mode = (1 << 1);

    for (uint8_t i = 0; i < 4; i++)
    {
        debe->layer_attr0_ctrl[i] = (i << 10) | ((i & 1) << 15);
        debe->layer_attr1_ctrl[i] = 0;
        de.layer[i].bits_per_pixel = 32;
        debe_layer_init(i);
        debe_layer_set_mode(i, DEBE_MODE_32BPP_RGB_888);
    }

    debe->reg_ctrl = (3 << 0);
}

static void tcon_deinit(void)
{
    //tcon->tcon0_ctrl = 0;
    tcon->ctrl = 0;
    tcon->int0 = 0;

    tcon->tcon0_dclk &= ~(0xf << 28);

    tcon->tcon0_io_tristate = 0xffffffff;
    tcon->tcon1_io_tristate = 0xffffffff;
}

static void tcon_clk_init(void)
{
    clk_tcon_config(CLK_VID_SRC_PLL_VIDEO_1X);
}

static void tcon_clk_enable(void)
{
    clk_enable(CCU_TCON_CLK, 31);
    clk_enable(CCU_BUS_CLK_GATE1, 4);
}

//static void tcon_clk_disable(void)
//{
//    clk_disable(CCU_TCON_CLK, 31);
//    clk_disable(CCU_BUS_CLK_GATE1, 4);
//}

static void defe_clk_init(void)
{
    clk_de_config(CCU_DEFE_CLK, CLK_DE_SRC_PLL_VIDEO, 1);
}
static void defe_clk_enable(void)
{
    clk_enable(CCU_DRAM_CLK_GATE, 24);
    clk_enable(CCU_DEFE_CLK, 31);
    clk_enable(CCU_BUS_CLK_GATE1, 14);
}

//static void defe_clk_disable(void)
//{
//    clk_disable(CCU_DRAM_CLK_GATE, 24);
//    clk_disable(CCU_DEFE_CLK, 31);
//    clk_disable(CCU_BUS_CLK_GATE1, 14);
//}

static void debe_clk_init(void)
{
    clk_de_config(CCU_DEBE_CLK, CLK_DE_SRC_PLL_VIDEO, 1);
}

static void debe_clk_enable(void)
{
    clk_enable(CCU_DRAM_CLK_GATE, 26);
    clk_enable(CCU_DEBE_CLK, 31);
    clk_enable(CCU_BUS_CLK_GATE1, 12);
}

//static void debe_clk_disable(void)
//{
//    clk_disable(CCU_DRAM_CLK_GATE, 26);
//    clk_disable(CCU_DEBE_CLK, 31);
//    clk_disable(CCU_BUS_CLK_GATE1, 12);
//}
