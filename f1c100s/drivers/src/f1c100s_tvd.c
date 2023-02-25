#include <stddef.h>
#include <stdlib.h>
#include "f1c100s_tvd.h"
#include "f1c100s_de.h"
#include "f1c100s_clock.h"
#include "io.h"

/* TODO:
 *
 * autoset
 * interrupts
 *
 * set_brightness
 * set_contrast
 * set_hue
 * set_saturation
 * set_sharpness
 *
 */

typedef struct {
    uint16_t width;
    uint16_t height;
    tvd_mode_e mode;
} tvd_params_t;

static tvd_params_t tvd;

static void tvd_dma_enable(void);
static void tvd_dma_disable(void);

void tvd_init(tvd_mode_e mode, void* buf_y, void* buf_c, uint8_t ch) {
    clk_enable(CCU_BUS_CLK_GATE1, 9); // TVD bus clock
    clk_enable(CCU_DRAM_CLK_GATE, 3); // DRAM access clock

    // Determine tvd clock division value. PLL_VIDEO should be configured and enabled!
    uint32_t tvd_clk_div = clk_pll_get_freq(PLL_VIDEO) / 27000000LU;

    clk_tvd_config(tvd_clk_div);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 9);

    tvd_set_out_buf(buf_y, buf_c);
    tvd_set_mode(mode);
    tvd_set_ch(ch);
}

void tvd_set_mode(tvd_mode_e mode) {
    write32(TVD_BASE + TVD_REG_E04, 0x8002AAA8);
    write32(TVD_BASE + TVD_REG_E2C, 0x00110000);
    write32(TVD_BASE + TVD_REG_040, 0x04000310);
    write32(TVD_BASE + TVD_REG_000, 0x00000000);
    write32(TVD_BASE + TVD_REG_014, 0x20000000);
    write32(TVD_BASE + TVD_REG_F24, 0x0682810A);

    write32(TVD_BASE + TVD_REG_F28, 0x00006440);
    write32(TVD_BASE + TVD_REG_F4C, 0x0E70106C);
    write32(TVD_BASE + TVD_REG_F54, 0x00000000);
    write32(TVD_BASE + TVD_REG_F58, 0x00000082);
    write32(TVD_BASE + TVD_REG_F6C, 0x00FFFAD0);
    write32(TVD_BASE + TVD_REG_F70, 0x0000A010);

    switch(mode) {
    case TVD_MODE_NTSC: // NTSC 720x480
        write32(TVD_BASE + TVD_REG_008, 0x00010001);
        write32(TVD_BASE + TVD_REG_00C, 0x00202068);
        write32(TVD_BASE + TVD_REG_010, 0x00300080);
        write32(TVD_BASE + TVD_REG_018, 0x21F07C1F);
        write32(TVD_BASE + TVD_REG_01C, 0x00820022);

        write32(TVD_BASE + TVD_REG_F08, 0x00590100);
        write32(TVD_BASE + TVD_REG_F0C, 0x00000010);
        write32(TVD_BASE + TVD_REG_F10, 0x008A32DD);
        write32(TVD_BASE + TVD_REG_F14, 0x800000A0);
        write32(TVD_BASE + TVD_REG_F1C, 0x008A0000);
        write32(TVD_BASE + TVD_REG_F2C, 0x0000CB74);
        write32(TVD_BASE + TVD_REG_F44, 0x00004632);
        write32(TVD_BASE + TVD_REG_F74, 0x000003C3);
        write32(TVD_BASE + TVD_REG_F80, 0x00500000);
        write32(TVD_BASE + TVD_REG_F84, 0x00610000);

        write32(TVD_BASE + TVD_REG_000, 0x00000001);

        tvd_set_out_size(720, 480);
        break;
    case TVD_MODE_PAL_B: // PAL-B/G 720x576
        write32(TVD_BASE + TVD_REG_008, 0x01101001);
        write32(TVD_BASE + TVD_REG_00C, 0x00202068);
        write32(TVD_BASE + TVD_REG_010, 0x00300050);
        write32(TVD_BASE + TVD_REG_018, 0x2A098ACB);
        write32(TVD_BASE + TVD_REG_01C, 0x0087002A);

        write32(TVD_BASE + TVD_REG_F08, 0x11590902);
        write32(TVD_BASE + TVD_REG_F0C, 0x00000016);
        write32(TVD_BASE + TVD_REG_F10, 0x008A32EC);
        write32(TVD_BASE + TVD_REG_F14, 0x800000A0);
        write32(TVD_BASE + TVD_REG_F1C, 0x00930000);
        write32(TVD_BASE + TVD_REG_F2C, 0x00000D74);
        write32(TVD_BASE + TVD_REG_F44, 0x0000412D);
        write32(TVD_BASE + TVD_REG_F74, 0x00000343);
        write32(TVD_BASE + TVD_REG_F80, 0x00500000);
        write32(TVD_BASE + TVD_REG_F84, 0x00C10000);

        write32(TVD_BASE + TVD_REG_000, 0x00000001);

        tvd_set_out_size(720, 576);
        break;
    case TVD_MODE_PAL_M: // PAL-M  - not tested
        write32(TVD_BASE + TVD_REG_008, 0x00002001);
        write32(TVD_BASE + TVD_REG_00C, 0x00002080);
        write32(TVD_BASE + TVD_REG_010, 0x00300080);
        write32(TVD_BASE + TVD_REG_018, 0x21E6EFE3);
        write32(TVD_BASE + TVD_REG_01C, 0x00820022);

        write32(TVD_BASE + TVD_REG_F08, 0x00590100);
        write32(TVD_BASE + TVD_REG_F0C, 0x00000040);
        write32(TVD_BASE + TVD_REG_F10, 0x008A32DD);
        write32(TVD_BASE + TVD_REG_F14, 0x800000A0);
        write32(TVD_BASE + TVD_REG_F1C, 0x008A0000);
        write32(TVD_BASE + TVD_REG_F2C, 0x0000CB74);
        write32(TVD_BASE + TVD_REG_F44, 0x00004632);
        write32(TVD_BASE + TVD_REG_F74, 0x000003C3);
        write32(TVD_BASE + TVD_REG_F80, 0x00500000);
        write32(TVD_BASE + TVD_REG_F84, 0x00610000);

        write32(TVD_BASE + TVD_REG_000, 0x00000001);

        tvd_set_out_size(720, 480);
        break;
    case TVD_MODE_PAL_N: // PAL-N - not tested
        write32(TVD_BASE + TVD_REG_008, 0x01103001);
        write32(TVD_BASE + TVD_REG_00C, 0x00002080);
        write32(TVD_BASE + TVD_REG_010, 0x00300080);
        write32(TVD_BASE + TVD_REG_018, 0x21F69446);
        write32(TVD_BASE + TVD_REG_01C, 0x00870026);

        write32(TVD_BASE + TVD_REG_F08, 0x11590902);
        write32(TVD_BASE + TVD_REG_F0C, 0x00000040);
        write32(TVD_BASE + TVD_REG_F10, 0x008A32EC);
        write32(TVD_BASE + TVD_REG_F14, 0x800000A0);
        write32(TVD_BASE + TVD_REG_F1C, 0x00DC0000);
        write32(TVD_BASE + TVD_REG_F2C, 0x00000D74);
        write32(TVD_BASE + TVD_REG_F44, 0x00004632);
        write32(TVD_BASE + TVD_REG_F74, 0x00000343);
        write32(TVD_BASE + TVD_REG_F80, 0x00500000);
        write32(TVD_BASE + TVD_REG_F84, 0x00C10000);

        write32(TVD_BASE + TVD_REG_000, 0x00000001);

        tvd_set_out_size(720, 576);
        break;
    case TVD_MODE_SECAM: // SECAM? 720x576 - not tested
        write32(TVD_BASE + TVD_REG_008, 0x01104001);
        write32(TVD_BASE + TVD_REG_00C, 0x00002080);
        write32(TVD_BASE + TVD_REG_010, 0x003100B0);
        write32(TVD_BASE + TVD_REG_018, 0x28A33BB2);
        write32(TVD_BASE + TVD_REG_01C, 0x00870026);

        write32(TVD_BASE + TVD_REG_F08, 0x11590902);
        write32(TVD_BASE + TVD_REG_F0C, 0x00000040);
        write32(TVD_BASE + TVD_REG_F10, 0x008A32EC);
        write32(TVD_BASE + TVD_REG_F14, 0x800000A0);
        write32(TVD_BASE + TVD_REG_F1C, 0x00DC0000);
        write32(TVD_BASE + TVD_REG_F2C, 0x00000D74);
        write32(TVD_BASE + TVD_REG_F44, 0x00005036);
        write32(TVD_BASE + TVD_REG_F74, 0x00000343);
        write32(TVD_BASE + TVD_REG_F80, 0x00500000);
        write32(TVD_BASE + TVD_REG_F84, 0x00C10000);

        write32(TVD_BASE + TVD_REG_000, 0x00000001);

        tvd_set_out_size(720, 576);
        break;
    default:
        break;
    }
    write32(TVD_BASE + TVD_REG_E2C, 0x60000000);
    tvd.mode = mode;
}

void tvd_set_out_buf(void* buf_y, void* buf_c) {
    write32(TVD_BASE + TVD_DMA_ADDR_Y, (uint32_t)buf_y);
    write32(TVD_BASE + TVD_DMA_ADDR_C, (uint32_t)buf_c);
    set32(TVD_BASE + TVD_DMA_CFG, (1 << 28)); // addr_valid
}

void tvd_set_out_size(uint16_t w, uint16_t h) {
    write32(TVD_BASE + TVD_DMA_SIZE, ((h / 2) << 16) | (w));
    write32(TVD_BASE + TVD_DMA_STRIDE, w);
    set32(TVD_BASE + TVD_DMA_CFG, (1 << 26)); // size_valid ?
    tvd.width  = w;
    tvd.height = h;
}

void tvd_get_out_size(uint16_t* w, uint16_t* h) {
    *w = tvd.width;
    *h = tvd.height;
}

// Set output format (4:2:0/4:2:2, planar(semi-planar?)/mb(packed?/macroblock?), U/V swap)
void tvd_set_out_fmt(tvd_out_fmt_e fmt) {
    uint32_t val = read32(TVD_BASE + TVD_DMA_CFG) & ~((1 << 4) | (1 << 24) | (1 << 8));
    write32(TVD_BASE + TVD_DMA_CFG, val | fmt);
}

void tvd_set_bluescreen_mode(tvd_blue_mode_e mode) {
    uint32_t val = read32(TVD_BASE + TVD_REG_F14) & ~(3 << 4);
    write32(TVD_BASE + TVD_REG_F14, val | (mode << 4));
}

static void tvd_dma_enable(void) {
    set32(TVD_BASE + TVD_DMA_CFG, (1 << 0));
}

static void tvd_dma_disable(void) {
    clear32(TVD_BASE + TVD_DMA_CFG, (1 << 0));
}

// Input channel select
void tvd_set_ch(uint8_t ch) {
    if(ch == 0)
        clear32(TVD_BASE + TVD_REG_E04, (1 << 0));
    else
        set32(TVD_BASE + TVD_REG_E04, (1 << 0));
}

uint8_t tvd_autoset(void) {
    return 0; // TODO:
}

uint32_t tvd_get_state(void) {
    return read32(TVD_BASE + TVD_STATE_0);
}

tvd_mode_e tvd_get_input_mode(void) {
    return 0; // TODO::
}

void tvd_enable(void) {
    tvd_dma_enable();
}

void tvd_disable(void) {
    tvd_dma_disable();
    // TODO:
}
