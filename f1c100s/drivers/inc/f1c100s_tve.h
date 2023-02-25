#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    TVE_ENABLE      = 0x000,
    TVE_CFG1        = 0x004,
    TVE_DAC1        = 0x008,
    TVE_NOTCH_DELAY = 0x00C,
    TVE_CHROMA_FREQ = 0x010,
    TVE_FB_PORCH    = 0x014,
    TVE_HD_VS       = 0x018,
    TVE_LINE_NUM    = 0x01C,
    TVE_LEVEL       = 0x020,
    TVE_DAC2        = 0x024,
    TVE_AUTO_EN     = 0x030,
    TVE_AUTO_ISR    = 0x034,
    TVE_AUTO_SR     = 0x038,
    TVE_AUTO_DEB    = 0x03C,
    TVE_CSC1        = 0x040,
    TVE_CSC2        = 0x044,
    TVE_CSC3        = 0x048,
    TVE_CSC4        = 0x04C,
    TVE_REG_0F8     = 0x0F8,
    TVE_REG_0FC     = 0x0FC,
    TVE_CB_RESET    = 0x100,
    TVE_VS_NUM      = 0x104,
    TVE_FILTER      = 0x108,
    TVE_CBCR_LEVEL  = 0x10C,
    TVE_TINT_PHASE  = 0x110,
    TVE_B_WIDTH     = 0x114,
    TVE_CBCR_GAIN   = 0x118,
    TVE_SYNC_LEVEL  = 0x11C,
    TVE_WHITE_LEVEL = 0x120,
    TVE_ACT_LINE    = 0x124,
    TVE_CHROMA_BW   = 0x128,
    TVE_CFG2        = 0x12C,
    TVE_RESYNC      = 0x130,
    TVE_SLAVE       = 0x134,
    TVE_CFG3        = 0x138,
    TVE_CFG4        = 0x13C,
} tve_reg_e;

typedef enum {
    TVE_MODE_NTSC,
    TVE_MODE_PAL,
} tve_mode_e;

void tve_init(tve_mode_e mode);

void tve_enable(void);

void tve_disable(void);

#ifdef __cplusplus
}
#endif
