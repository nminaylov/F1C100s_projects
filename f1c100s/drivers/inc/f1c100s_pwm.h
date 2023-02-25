#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    PWM0 = 0,
    PWM1 = 1,
} pwm_ch_e;

typedef enum {
    PWM_CTRL = 0x00,
    PWM_CH0  = 0x04,
    PWM_CH1  = 0x08,
} pwm_reg_e;

typedef enum {
    PWM_MODE_CONTINUOUS   = 0,
    PWM_MODE_SINGLE_PULSE = (1 << 7),
    PWM_MODE_DIRECT_24MHZ = (1 << 9),
} pwm_mode_e;

typedef enum {
    PWM_PSC_120   = 0,
    PWM_PSC_180   = 1,
    PWM_PSC_240   = 2,
    PWM_PSC_360   = 3,
    PWM_PSC_480   = 4,
    PWM_PSC_12000 = 8,
    PWM_PSC_24000 = 9,
    PWM_PSC_36000 = 10,
    PWM_PSC_48000 = 11,
    PWM_PSC_72000 = 12,
    PWM_PSC_1     = 15,
} pwm_prescaller_e;

void pwm_init(uint8_t ch, pwm_mode_e mode, uint8_t active_level, pwm_prescaller_e psc);

void pwm_set_period(uint8_t ch, uint16_t val);

void pwm_set_pulse_len(uint8_t ch, uint16_t val);

void pwm_enable(uint8_t ch);

void pwm_disable(uint8_t ch);

void pwm_pulse_start(uint8_t ch);

#ifdef __cplusplus
}
#endif
