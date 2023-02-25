#include <stddef.h>
#include <stdlib.h>
#include "f1c100s_tve.h"
#include "f1c100s_clock.h"
#include "io.h"

void tve_init(tve_mode_e mode) {
    // Determine tve clock division value. PLL_VIDEO should be configured and enabled!
    uint32_t tve_clk_div = clk_pll_get_freq(PLL_VIDEO) / 27000000LU;
    clk_tve_config(tve_clk_div);
    clk_enable(CCU_BUS_CLK_GATE1, 10);
    clk_reset_clear(CCU_BUS_SOFT_RST1, 10);

    write32(TVE_BASE + TVE_DAC1, 0x433810A1);
    if(mode == TVE_MODE_NTSC) { // NTSC
        write32(TVE_BASE + TVE_CFG1, 0x07030000);
        write32(TVE_BASE + TVE_NOTCH_DELAY, 0x00000120);
        write32(TVE_BASE + TVE_CHROMA_FREQ, 0x21F07C1F);
        write32(TVE_BASE + TVE_FB_PORCH, 0x00760020);
        write32(TVE_BASE + TVE_HD_VS, 0x00000016);
        write32(TVE_BASE + TVE_LINE_NUM, 0x0016020D);
        write32(TVE_BASE + TVE_LEVEL, 0x00F0011A);
        write32(TVE_BASE + TVE_CB_RESET, 0x00000001);
        write32(TVE_BASE + TVE_VS_NUM, 0x00000000);
        write32(TVE_BASE + TVE_FILTER, 0x00000002);
        write32(TVE_BASE + TVE_CBCR_LEVEL, 0x0000004F);
        write32(TVE_BASE + TVE_TINT_PHASE, 0x00000000);
        write32(TVE_BASE + TVE_B_WIDTH, 0x0016447E);
        write32(TVE_BASE + TVE_CBCR_GAIN, 0x0000A0A0);
        write32(TVE_BASE + TVE_SYNC_LEVEL, 0x001000F0);
        write32(TVE_BASE + TVE_WHITE_LEVEL, 0x01E80320);
        write32(TVE_BASE + TVE_ACT_LINE, 0x000005A0);
        write32(TVE_BASE + TVE_CHROMA_BW, 0x00000000);
        write32(TVE_BASE + TVE_CFG2, 0x00000101);
        write32(TVE_BASE + TVE_RESYNC, 0x000E000C);
        write32(TVE_BASE + TVE_SLAVE, 0x00000000);
        write32(TVE_BASE + TVE_CFG3, 0x00000000);
        write32(TVE_BASE + TVE_CFG4, 0x00000000);
    } else { // PAL
        write32(TVE_BASE + TVE_CFG1, 0x07030001);
        write32(TVE_BASE + TVE_NOTCH_DELAY, 0x00000120);
        write32(TVE_BASE + TVE_CHROMA_FREQ, 0x2A098ACB);
        write32(TVE_BASE + TVE_FB_PORCH, 0x008A0018);
        write32(TVE_BASE + TVE_HD_VS, 0x00000016);
        write32(TVE_BASE + TVE_LINE_NUM, 0x00160271);
        write32(TVE_BASE + TVE_LEVEL, 0x00FC00FC);
        write32(TVE_BASE + TVE_CB_RESET, 0x00000000);
        write32(TVE_BASE + TVE_VS_NUM, 0x00000001);
        write32(TVE_BASE + TVE_FILTER, 0x00000005);
        write32(TVE_BASE + TVE_CBCR_LEVEL, 0x00002828);
        write32(TVE_BASE + TVE_TINT_PHASE, 0x00000000);
        write32(TVE_BASE + TVE_B_WIDTH, 0x0016447E);
        write32(TVE_BASE + TVE_CBCR_GAIN, 0x0000E0E0);
        write32(TVE_BASE + TVE_SYNC_LEVEL, 0x001000F0);
        write32(TVE_BASE + TVE_WHITE_LEVEL, 0x01E80320);
        write32(TVE_BASE + TVE_ACT_LINE, 0x000005A0);
        write32(TVE_BASE + TVE_CHROMA_BW, 0x00000000);
        write32(TVE_BASE + TVE_CFG2, 0x00000101);
        write32(TVE_BASE + TVE_RESYNC, 0x800D000C);
        write32(TVE_BASE + TVE_SLAVE, 0x00000000);
        write32(TVE_BASE + TVE_CFG3, 0x00000000);
        write32(TVE_BASE + TVE_CFG4, 0x00000000);
    }
    write32(TVE_BASE + TVE_ENABLE, 1);
}

void tve_enable(void) {
    set32(TVE_BASE + TVE_DAC1, 0x1 << 0);
    set32(TVE_BASE + TVE_ENABLE, 0x1 << 0);
}

void tve_disable(void) {
    clear32(TVE_BASE + TVE_DAC1, 0x1 << 0);
    clear32(TVE_BASE + TVE_ENABLE, 0x1 << 0);
}
