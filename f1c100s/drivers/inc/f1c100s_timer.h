#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    TIM0 = 0,
    TIM1 = 1,
    TIM2 = 2,
} tim_ch_e;

typedef enum {
    TIM_IRQ_EN  = 0x00,
    TIM_IRQ_STA = 0x04,
    TIM_0_CTRL  = 0x10,
    TIM_0_INTV  = 0x14,
    TIM_0_CUR   = 0x18,
    TIM_1_CTRL  = 0x20,
    TIM_1_INTV  = 0x24,
    TIM_1_CUR   = 0x28,
    TIM_2_CTRL  = 0x30,
    TIM_2_INTV  = 0x34,
    TIM_2_CUR   = 0x38,

    AVS_CTRL = 0x80,
    AVS_CNT0 = 0x84,
    AVS_CNT1 = 0x88,
    AVS_DIV  = 0x8C,

    WDG_IRQ_EN  = 0xA0,
    WDG_IRQ_STA = 0xA4,
    WDG_CTRL    = 0xB0,
    WDG_CFG     = 0xB4,
    WDG_MODE    = 0xB8,
} tim_reg_e;

typedef enum {
    TIM_MODE_CONT   = 0,
    TIM_MODE_SINGLE = 1,
} tim_mode_e;

typedef enum {
    TIM_SRC_LOSC = 0,
    TIM_SRC_HOSC = 1,
} tim_source_e;

typedef enum {
    TIM_PSC_1   = 0,
    TIM_PSC_2   = 1,
    TIM_PSC_4   = 2,
    TIM_PSC_8   = 3,
    TIM_PSC_16  = 4,
    TIM_PSC_32  = 5,
    TIM_PSC_64  = 6,
    TIM_PSC_128 = 7,
} tim_prescaller_e;

typedef enum {
    WDG_MODE_RESET = 1,
    WDG_MODE_INT   = 2,
} wdg_mode_e;

typedef enum {
    WDG_INTV_500MS = 0,
    WDG_INTV_1S    = 1,
    WDG_INTV_2S    = 2,
    WDG_INTV_3S    = 3,
    WDG_INTV_4S    = 4,
    WDG_INTV_5S    = 5,
    WDG_INTV_6S    = 6,
    WDG_INTV_8S    = 7,
    WDG_INTV_10S   = 8,
    WDG_INTV_12S   = 9,
    WDG_INTV_14S   = 10,
    WDG_INTV_16S   = 11,
} wdg_period_e;

void tim_init(uint8_t ch, tim_mode_e mode, tim_source_e src, tim_prescaller_e psc);

void tim_set_period(uint8_t ch, uint32_t val);

uint32_t tim_get_cnt(uint8_t ch);

void tim_set_cnt(uint8_t ch, uint32_t val);

void tim_start(uint8_t ch);

void tim_stop(uint8_t ch);

void tim_reload(uint8_t ch);

void tim_int_enable(uint8_t ch);

void tim_int_disable(uint8_t ch);

uint8_t tim_get_int_status(void);

void tim_clear_irq(uint8_t ch);

void wdg_init(wdg_mode_e mode, wdg_period_e period);

void wdg_disable(void);

void wdg_feed(void);

uint8_t wdg_get_int_status(void);

#ifdef __cplusplus
}
#endif
