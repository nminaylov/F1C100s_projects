#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    CCU_PLL_CPU_CTRL    = 0x000,
    CCU_PLL_AUDIO_CTRL  = 0x008,
    CCU_PLL_VIDEO_CTRL  = 0x010,
    CCU_PLL_VE_CTRL     = 0x018,
    CCU_PLL_DDR_CTRL    = 0x020,
    CCU_PLL_PERIPH_CTRL = 0x028,
    CCU_CPU_CFG         = 0x050,
    CCU_AHB_APB_CFG     = 0x054,

    CCU_BUS_CLK_GATE0 = 0x060,
    CCU_BUS_CLK_GATE1 = 0x064,
    CCU_BUS_CLK_GATE2 = 0x068,

    CCU_SDMMC0_CLK      = 0x088,
    CCU_SDMMC1_CLK      = 0x08c,
    CCU_DAUDIO_CLK      = 0x0b0,
    CCU_SPDIF_CLK       = 0x0b4,
    CCU_I2S_CLK         = 0x0b8,
    CCU_USBPHY_CFG      = 0x0cc,
    CCU_DRAM_CLK_GATE   = 0x100,
    CCU_DEBE_CLK        = 0x104,
    CCU_DEFE_CLK        = 0x10c,
    CCU_TCON_CLK        = 0x118,
    CCU_DEINTERLACE_CLK = 0x11c,
    CCU_TVE_CLK         = 0x120,
    CCU_TVD_CLK         = 0x124,
    CCU_CSI_CLK         = 0x134,
    CCU_VE_CLK          = 0x13c,
    CCU_ADDA_CLK        = 0x140,
    CCU_AVS_CLK         = 0x144,

    CCU_PLL_STABLE_TIME0 = 0x200,
    CCU_PLL_STABLE_TIME1 = 0x204,
    CCU_PLL_CPU_BIAS     = 0x220,
    CCU_PLL_AUDIO_BIAS   = 0x224,
    CCU_PLL_VIDEO_BIAS   = 0x228,
    CCU_PLL_VE_BIAS      = 0x22c,
    CCU_PLL_DDR0_BIAS    = 0x230,
    CCU_PLL_PERIPH_BIAS  = 0x234,
    CCU_PLL_CPU_TUN      = 0x250,
    CCU_PLL_DDR_TUN      = 0x260,
    CCU_PLL_AUDIO_PAT    = 0x284,
    CCU_PLL_VIDEO_PAT    = 0x288,
    CCU_PLL_DDR0_PAT     = 0x290,

    CCU_BUS_SOFT_RST0 = 0x2c0,
    CCU_BUS_SOFT_RST1 = 0x2c4,
    CCU_BUS_SOFT_RST2 = 0x2d0,
} ccu_reg_e;

typedef enum {
    PLL_CPU    = CCU_PLL_CPU_CTRL,
    PLL_AUDIO  = CCU_PLL_AUDIO_CTRL,
    PLL_VIDEO  = CCU_PLL_VIDEO_CTRL,
    PLL_VE     = CCU_PLL_VE_CTRL,
    PLL_DDR    = CCU_PLL_DDR_CTRL,
    PLL_PERIPH = CCU_PLL_PERIPH_CTRL,
} pll_ch_e;

typedef enum {
    CLK_CPU_SRC_LOSC    = 0, // not used?
    CLK_CPU_SRC_OSC24M  = 1,
    CLK_CPU_SRC_PLL_CPU = 2,
} clk_source_cpu_e;

typedef enum {
    CLK_AHB_SRC_LOSC              = 0,
    CLK_AHB_SRC_OSC24M            = 1,
    CLK_AHB_SRC_CPUCLK            = 2,
    CLK_AHB_SRC_PLL_PERIPH_PREDIV = 3,
} clk_source_ahb_e;

typedef enum {
    CLK_APB_DIV_2 = 1,
    CLK_APB_DIV_4 = 2,
    CLK_APB_DIV_8 = 3,
} clk_div_apb_e;

typedef enum {
    CLK_DE_SRC_PLL_VIDEO  = 0,
    CLK_DE_SRC_PLL_PERIPH = 2,
} clk_source_de_e;

typedef enum {
    CLK_SDC_SRC_OSC24M     = 0,
    CLK_SDC_SRC_PLL_PERIPH = 1,
} clk_source_sdc_e;

typedef enum {
    CLK_VID_SRC_PLL_VIDEO_1X = 0,
    CLK_VID_SRC_OSC24M       = 1, // TVD only
    CLK_VID_SRC_PLL_VIDEO_2X = 2,
} clk_source_vid_e;

void clk_pll_enable(pll_ch_e pll);

void clk_pll_disable(pll_ch_e pll);

uint8_t clk_pll_is_locked(pll_ch_e pll);

void clk_pll_init(pll_ch_e pll, uint8_t mul, uint8_t div);

uint32_t clk_pll_get_freq(pll_ch_e pll);

void clk_enable(uint32_t reg, uint8_t bit);

void clk_disable(uint32_t reg, uint8_t bit);

void clk_cpu_config(clk_source_cpu_e source);

void clk_hclk_config(uint8_t div);

void clk_ahb_config(clk_source_ahb_e src, uint8_t prediv, uint8_t div);

void clk_apb_config(clk_div_apb_e div);

void clk_de_config(uint32_t reg, clk_source_de_e source, uint8_t div);

void clk_tcon_config(clk_source_vid_e source);

void clk_tve_config(uint8_t div);

void clk_tvd_config(uint8_t div);

uint32_t clk_sdc_config(uint32_t reg, uint32_t freq);

uint32_t clk_cpu_get_freq(void);

uint32_t clk_hclk_get_freq(void);

uint32_t clk_ahb_get_freq(void);

uint32_t clk_apb_get_freq(void);

void clk_reset_set(uint32_t reg, uint8_t bit);

void clk_reset_clear(uint32_t reg, uint8_t bit);

#ifdef __cplusplus
}
#endif
