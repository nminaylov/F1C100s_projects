#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    TVD_REG_000 = 0x000,
    TVD_REG_004 = 0x004,
    TVD_REG_008 = 0x008,
    TVD_REG_00C = 0x00C, // sharpness, brightness, contrast
    TVD_REG_010 = 0x010, // chroma_enhance, hue, saturation
    TVD_REG_014 = 0x014, // prescaler?
    TVD_REG_018 = 0x018, // chroma_freq/27000000*(2^32)
    TVD_REG_01C = 0x01C,

    TVD_REG_040 = 0x040,
    TVD_REG_048 = 0x048,
    TVD_REG_04C = 0x04C,
    TVD_REG_050 = 0x050,
    TVD_REG_054 = 0x054,
    TVD_REG_058 = 0x058,
    TVD_REG_05C = 0x05C,
    TVD_REG_060 = 0x060,
    TVD_REG_064 = 0x064,
    TVD_REG_068 = 0x068,
    TVD_REG_06C = 0x06C,
    TVD_REG_070 = 0x070,

    TVD_DMA_ADDR_Y = 0x080,
    TVD_DMA_ADDR_C = 0x084,
    TVD_DMA_CFG    = 0x088,
    TVD_DMA_SIZE   = 0x08C,
    TVD_DMA_STRIDE = 0x090,
    TVD_DMA_IRQ0   = 0x094, // irq_st?
    TVD_DMA_IRQ1   = 0x09C, // irq_en?

    TVD_REG_0B0 = 0x0B0, // ffffffff
    TVD_REG_0B4 = 0x0B4, // ffffffff

    TVD_REG_E04 = 0xE04, // .0 - input selection
    TVD_REG_E0C = 0xE0C,
    TVD_REG_E2C = 0xE2C,
    TVD_REG_E30 = 0xE30,

    TVD_STATE_0 = 0xE40, // State flags
    TVD_STATE_1 = 0xE44, //
    TVD_STATE_2 = 0xE48, // measured chroma freq?
    TVD_STATE_3 = 0xE4C, //
    TVD_STATE_4 = 0xE50, //

    TVD_REG_F08 = 0xF08,
    TVD_REG_F0C = 0xF0C,
    TVD_REG_F10 = 0xF10,
    TVD_REG_F14 = 0xF14,
    TVD_REG_F18 = 0xF18,
    TVD_REG_F1C = 0xF1C,
    TVD_REG_F20 = 0xF20,
    TVD_REG_F24 = 0xF24,
    TVD_REG_F28 = 0xF28,
    TVD_REG_F2C = 0xF2C,
    TVD_REG_F30 = 0xF30,
    TVD_REG_F34 = 0xF34,
    TVD_REG_F38 = 0xF38,
    TVD_REG_F3C = 0xF3C,
    TVD_REG_F40 = 0xF40,
    TVD_REG_F44 = 0xF44,
    TVD_REG_F48 = 0xF48,
    TVD_REG_F4C = 0xF4C,
    TVD_REG_F50 = 0xF50,
    TVD_REG_F54 = 0xF54,
    TVD_REG_F58 = 0xF58,
    TVD_REG_F5C = 0xF5C,
    TVD_REG_F60 = 0xF60,
    TVD_REG_F64 = 0xF64,
    TVD_REG_F68 = 0xF68,
    TVD_REG_F6C = 0xF6C,
    TVD_REG_F70 = 0xF70,
    TVD_REG_F74 = 0xF74,
    TVD_REG_F78 = 0xF78,
    TVD_REG_F7C = 0xF7C,
    TVD_REG_F80 = 0xF80,
    TVD_REG_F84 = 0xF84,
} tvd_reg_e;

typedef enum {
    TVD_MODE_UNKNOWN,
    TVD_MODE_NTSC,
    TVD_MODE_PAL_B,
    TVD_MODE_PAL_M,
    TVD_MODE_PAL_N,
    TVD_MODE_SECAM,
} tvd_mode_e;

typedef enum {
    TVD_BLUE_OFF      = 0,
    TVD_BLUE_FORCE_ON = 1,
    TVD_BLUE_AUTO     = 2, // On, if no signal
} tvd_blue_mode_e;

typedef enum {
    TVD_FMT_420_PL = (0UL << 4) | (0UL << 24),
    TVD_FMT_420_MB = (0UL << 4) | (1UL << 24),
    TVD_FMT_422_PL = (1UL << 4) | (0UL << 24),
    TVD_FMT_422_MB = (1UL << 4) | (1UL << 24),

    TVD_FMT_SWAP_UV = (1UL << 8),
} tvd_out_fmt_e;

// Status bits in reg E40
typedef enum {
    TVD_ST_NOISY      = (1 << 19),
    TVD_ST_625_LINES  = (1 << 18),
    TVD_ST_SECAM      = (1 << 17),
    TVD_ST_PAL        = (1 << 16),
    TVD_ST_V_NON_STD  = (1 << 10),
    TVD_ST_H_NON_STD  = (1 << 9),
    TVD_ST_PROG_SCAN  = (1 << 8),
    TVD_ST_C_PLL_LOCK = (1 << 3),
    TVD_ST_V_LOCK     = (1 << 2),
    TVD_ST_H_LOCK     = (1 << 1),
    TVD_ST_NO_SIGNAL  = (1 << 0),
} tvd_state_e;

void tvd_init(tvd_mode_e mode, void* buf_y, void* buf_c, uint8_t ch);

void tvd_set_mode(tvd_mode_e mode);

void tvd_set_out_buf(void* buf_y, void* buf_c);

void tvd_set_out_size(uint16_t w, uint16_t h);

void tvd_set_out_fmt(tvd_out_fmt_e fmt);

void tvd_set_bluescreen_mode(tvd_blue_mode_e mode);

void tvd_set_ch(uint8_t ch);

void tvd_get_out_size(uint16_t* w, uint16_t* h);

uint32_t tvd_get_state(void);

void tvd_enable(void);

void tvd_disable(void);

#ifdef __cplusplus
}
#endif
