#include "f1c100s_clock.h"
#include "io.h"

static void pll_cpu_init(uint8_t mul, uint8_t div);
static uint32_t pll_cpu_get_freq(void);
static void pll_audio_init(uint16_t mul, uint8_t div);
static uint32_t pll_audio_get_freq(void);
static void pll_video_init(pll_ch_e pll, uint8_t mul, uint8_t div);
static uint32_t pll_video_get_freq(pll_ch_e pll);
static void pll_periph_init(uint8_t mul, uint8_t div);
static uint32_t pll_periph_get_freq(void);
static uint32_t pll_ddr_get_freq(void);

/************** PLLs ***************/
// Enable PLL
inline void clk_pll_enable(pll_ch_e pll) {
    write32(CCU_BASE + pll, (read32(CCU_BASE + pll) | (1 << 31)));
}

// Disable PLL
inline void clk_pll_disable(pll_ch_e pll) {
    write32(CCU_BASE + pll, (read32(CCU_BASE + pll) & ~(1 << 31)));
}

// Get PLL lock state
uint8_t clk_pll_is_locked(pll_ch_e pll) {
    uint32_t val = read32(CCU_BASE + pll);
    return ((val >> 28) & 0x1);
}

// PLL channel configuration
// out = 24MHz * mul / div
void clk_pll_init(pll_ch_e pll, uint8_t mul, uint8_t div) {
    switch(pll) {
    case PLL_CPU:
        pll_cpu_init(mul, div);
        break;
    case PLL_AUDIO:
        pll_audio_init(mul, div);
        break;
    case PLL_VIDEO:
    case PLL_VE:
        pll_video_init(pll, mul, div);
        break;
    case PLL_DDR:
        break; // TODO:
    case PLL_PERIPH:
        pll_periph_init(mul, div);
        break;
    default:
        break;
    }
}

uint32_t clk_pll_get_freq(pll_ch_e pll) {
    if(!clk_pll_is_locked(pll)) return 0;

    switch(pll) {
    case PLL_CPU:
        return pll_cpu_get_freq();
    case PLL_AUDIO:
        return pll_audio_get_freq();
    case PLL_VIDEO:
    case PLL_VE:
        return pll_video_get_freq(pll);
    case PLL_DDR:
        return pll_ddr_get_freq();
    case PLL_PERIPH:
        return pll_periph_get_freq();
    default:
        break;
    }
    return 0;
}

// out = (24MHz*N*K) / (M*P)
static void pll_cpu_init(uint8_t mul, uint8_t div) {
    if((mul == 0) || (div == 0)) return;
    if((mul > 128) || (div > 16)) return;

    uint8_t n, k, m, p;
    // mul = n*k
    // n = 1..32
    // k = 1..4
    for(k = 1; k <= 4; k++) {
        n = mul / k;
        if((n < 32) && (n * k == mul)) break;
    }
    if(n * k != mul) return;
    // div = m*p
    // m = 1..4
    // k = 1,2,4
    for(m = 1; m <= 4; m++) {
        p = div / m;
        if(((p == 1) || (p == 2) || (p == 4)) && (m * p == div)) break;
    }
    if(m * p != div) return;

    p--;
    if(p == 3) p = 2;

    uint32_t val = read32(CCU_BASE + CCU_PLL_CPU_CTRL);
    val &= (1 << 31) | (1 << 28);
    val |= ((n - 1) << 8) | ((k - 1) << 4) | (m - 1) | (p << 16);
    write32(CCU_BASE + CCU_PLL_CPU_CTRL, val);
}

static uint32_t pll_cpu_get_freq(void) {
    uint32_t reg = read32(CCU_BASE + CCU_PLL_CPU_CTRL);

    uint32_t n = (reg >> 8) & 0x1F;
    uint32_t k = (reg >> 4) & 0x3;
    uint32_t m = (reg >> 0) & 0x3;
    uint32_t p = (reg >> 16) & 0x3;

    p = (1 << p);

    return (24000000 * (n + 1) * (k + 1) / ((m + 1) * p));
}

// out = (24MHz*N*2) / M
static void pll_audio_init(uint16_t mul, uint8_t div) {
    if((mul == 0) || (div == 0)) return;
    if((mul > 256) || (div > 32)) return;

    uint8_t n = (uint8_t)(mul / 2); // mul = n*2
    // div = m

    uint32_t val = read32(CCU_BASE + CCU_PLL_AUDIO_CTRL);
    val &= (1 << 31) | (1 << 28);
    val |= ((n - 1) << 8) | (div - 1);
    write32(CCU_BASE + CCU_PLL_AUDIO_CTRL, val);
}

static uint32_t pll_audio_get_freq(void) {
    uint32_t reg = read32(CCU_BASE + CCU_PLL_AUDIO_CTRL);

    uint32_t mul = (reg >> 8) & 0x7F;
    uint32_t div = (reg >> 0) & 0x1F;

    if(reg & (1 << 24)) // SDM
        mul &= 0xF;

    return (24000000 * 2 * (mul + 1) / (div + 1));
}

// out = (24MHz*N) / M
static void pll_video_init(pll_ch_e pll, uint8_t mul, uint8_t div) {
    if((mul == 0) || (div == 0)) return;
    if((mul > 128) || (div > 16)) return;

    // mul = n
    // div = m

    uint32_t val = read32(CCU_BASE + pll);
    val &= (1 << 31) | (1 << 28);
    val |= ((mul - 1) << 8) | (div - 1) | (1 << 24);
    write32(CCU_BASE + pll, val);
}

static uint32_t pll_video_get_freq(pll_ch_e pll) {
    uint32_t reg = read32(CCU_BASE + pll);

    if((reg & (1 << 24)) == 0) {
        // Fractional mode
        if(reg & (1 << 25))
            return 297000000;
        else
            return 270000000;
    } else {
        // Integer mode
        uint32_t mul = (reg >> 8) & 0x7F;
        uint32_t div = (reg >> 0) & 0xF;

        return (24000000 * (mul + 1) / (div + 1));
    }
}

// out = (24MHz*N) / M
static void pll_periph_init(uint8_t mul, uint8_t div) {
    if((mul == 0) || (div == 0)) return;
    if((mul > 32) || (div > 4)) return;

    // mul = n
    // div = m

    uint32_t val = read32(CCU_BASE + CCU_PLL_PERIPH_CTRL);
    val &= (1 << 31) | (1 << 28);
    val |= ((mul - 1) << 8) | ((div - 1) << 4) | (1 << 18); // do we need 24m output?
    write32(CCU_BASE + CCU_PLL_PERIPH_CTRL, val);
}

static uint32_t pll_periph_get_freq(void) {
    uint32_t reg = read32(CCU_BASE + CCU_PLL_PERIPH_CTRL);

    uint32_t mul = (reg >> 8) & 0x1F;
    uint32_t div = (reg >> 4) & 0x3;

    return (24000000 * (mul + 1) / (div + 1));
}

static uint32_t pll_ddr_get_freq(void) {
    uint32_t reg = read32(CCU_BASE + CCU_PLL_DDR_CTRL);

    uint32_t n = (reg >> 8) & 0x1F;
    uint32_t k = (reg >> 4) & 0x3;
    uint32_t m = (reg >> 0) & 0x3;

    return (24000000 * (n + 1) * (k + 1) / (m + 1));
}

/************** Clock gating ***************/

inline void clk_enable(uint32_t reg, uint8_t bit) {
    set32(CCU_BASE + reg, (1 << bit));
}

inline void clk_disable(uint32_t reg, uint8_t bit) {
    clear32(CCU_BASE + reg, (1 << bit));
}

/************** Specific clocks configuration ***************/

// CPU clock configuration
void clk_cpu_config(clk_source_cpu_e source) {
    uint32_t reg = read32(CCU_BASE + CCU_CPU_CFG) & ~(0x3 << 16);
    write32(CCU_BASE + CCU_CPU_CFG, reg | (source << 16));
}

uint32_t clk_cpu_get_freq(void) {
    clk_source_cpu_e src = (read32(CCU_BASE + CCU_CPU_CFG) >> 16) & 0x3;

    switch(src) {
    case CLK_CPU_SRC_LOSC:
        return 32000; // ??
    case CLK_CPU_SRC_OSC24M:
        return 24000000;
    case CLK_CPU_SRC_PLL_CPU:
        return clk_pll_get_freq(PLL_CPU);
    default:
        return 0;
    }
}

// HCLK = CPUCLK / div
void clk_hclk_config(uint8_t div) {
    if((div == 0) || (div > 4)) return;

    uint32_t val = read32(CCU_BASE + CCU_AHB_APB_CFG) & ~(0x3 << 16);
    write32(CCU_BASE + CCU_AHB_APB_CFG, val | ((div - 1) << 16));
}

uint32_t clk_hclk_get_freq(void) {
    uint8_t div = (read32(CCU_BASE + CCU_AHB_APB_CFG) >> 16) & 0x3;

    return (clk_cpu_get_freq() / (div + 1));
}

// AHB = (src or src/prediv)/div
void clk_ahb_config(clk_source_ahb_e src, uint8_t prediv, uint8_t div) {
    if((prediv == 0) || (prediv > 4)) return;
    if((div == 0) || ((div > 4) && (div != 8)) || (div == 3)) return;
    if(div == 4) div = 3;
    if(div == 8) div = 4;

    uint32_t val = read32(CCU_BASE + CCU_AHB_APB_CFG) & ~((0x3 << 12) | (0xF << 4));
    write32(
        CCU_BASE + CCU_AHB_APB_CFG, val | (src << 12) | ((prediv - 1) << 6) | ((div - 1) << 4));
}

uint32_t clk_ahb_get_freq(void) {
    clk_source_ahb_e src = (read32(CCU_BASE + CCU_AHB_APB_CFG) >> 12) & 0x3;

    uint8_t div    = (read32(CCU_BASE + CCU_AHB_APB_CFG) >> 4) & 0x3;
    uint8_t prediv = (read32(CCU_BASE + CCU_AHB_APB_CFG) >> 6) & 0x3;

    div = (1 << div);

    switch(src) {
    case CLK_AHB_SRC_LOSC:
        return (32000 / div);
    case CLK_AHB_SRC_OSC24M:
        return (24000000 / div);
    case CLK_AHB_SRC_CPUCLK:
        return (clk_cpu_get_freq() / div);
    case CLK_AHB_SRC_PLL_PERIPH_PREDIV:
        return (clk_pll_get_freq(PLL_PERIPH) / (prediv + 1) / div);
    default:
        return 0;
    }
}

// APB = AHB / div
void clk_apb_config(clk_div_apb_e div) {
    uint32_t val = read32(CCU_BASE + CCU_AHB_APB_CFG) & ~(0x3 << 8);
    write32(CCU_BASE + CCU_AHB_APB_CFG, val | (div << 8));
}

uint32_t clk_apb_get_freq(void) {
    clk_div_apb_e div = (read32(CCU_BASE + CCU_AHB_APB_CFG) >> 8) & 0x3;

    switch(div) {
    case CLK_APB_DIV_4:
        return clk_ahb_get_freq() / 4;
    case CLK_APB_DIV_8:
        return clk_ahb_get_freq() / 8;
    default:
        return clk_ahb_get_freq() / 2;
    }
}

// DEBE / DEFE clock configuration
void clk_de_config(uint32_t reg, clk_source_de_e source, uint8_t div) {
    if((div == 0) || (div > 16)) return;

    uint32_t val = read32(CCU_BASE + reg);

    val &= ~((0x7 << 24) | (0xF));
    val |= (source << 24) | (div - 1);

    write32(CCU_BASE + reg, val);
}

// TCON clock configuration
void clk_tcon_config(clk_source_vid_e source) {
    uint32_t val = read32(CCU_BASE + CCU_TCON_CLK) & ~(0x7 << 24);
    write32(CCU_BASE + CCU_TCON_CLK, val | (source << 24));
}

// Video encoder clock configuration
void clk_tve_config(uint8_t div) { // TODO: source select
    if((div == 0) || (div > 16)) return;
    write32(CCU_BASE + CCU_TVE_CLK, (0x80008100) | (div - 1));
}

// Video decoder clock configuration
void clk_tvd_config(uint8_t div) { // TODO: source select
    if((div == 0) || (div > 16)) return;
    write32(CCU_BASE + CCU_TVD_CLK, (0x80000000) | (div - 1));
}

// SD card controller clock
uint32_t clk_sdc_config(uint32_t reg, uint32_t freq) {
    uint32_t in_freq = 0;
    uint32_t reg_val = (1 << 31);

    if(freq <= 24000000) {
        reg_val |= (0 << 24); // OSC24M
        in_freq = 24000000;
    } else {
        reg_val |= (1 << 24); // PLL_PERIPH
        in_freq = clk_pll_get_freq(PLL_PERIPH);
    }

    uint8_t div = in_freq / freq;
    if(in_freq % freq) div++;

    uint8_t prediv = 0;
    while(div > 16) {
        prediv++;
        if(prediv > 3) return 0;
        div = (div + 1) / 2;
    }

    /* determine delays */
    uint8_t samp_phase = 0;
    uint8_t out_phase  = 0;
    if(freq <= 400000) {
        out_phase  = 0;
        samp_phase = 0;
    } else if(freq <= 25000000) {
        out_phase  = 0;
        samp_phase = 5;
    } else if(freq <= 52000000) {
        out_phase  = 3;
        samp_phase = 4;
    } else { /* freq > 52000000 */
        out_phase  = 1;
        samp_phase = 4;
    }
    reg_val |= (samp_phase << 20) | (out_phase << 8);
    reg_val |= (prediv << 16) | ((div - 1) << 0);

    write32(CCU_BASE + reg, reg_val);

    return in_freq / div;
}

/************** Resets ***************/

inline void clk_reset_set(uint32_t reg, uint8_t bit) {
    clear32(CCU_BASE + reg, (1 << bit));
}

inline void clk_reset_clear(uint32_t reg, uint8_t bit) {
    set32(CCU_BASE + reg, (1 << bit));
}
