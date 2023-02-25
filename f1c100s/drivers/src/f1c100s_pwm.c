#include "f1c100s_pwm.h"
#include "io.h"

void pwm_init(uint8_t ch, pwm_mode_e mode, uint8_t active_level, pwm_prescaller_e psc) {
    pwm_disable(ch);
    uint32_t val = read32(PWM_BASE);
    val &= ~(0x3FF << (ch * 15));
    val |= (mode | (active_level << 5) | psc) << (ch * 15);
    write32(PWM_BASE, val);
}

inline void pwm_set_period(uint8_t ch, uint16_t val) {
    uint32_t reg  = PWM_BASE + PWM_CH0 + ch * 4;
    uint32_t temp = read32(reg) & ~(0xFFFF << 16);
    write32(reg, temp | ((uint32_t)val << 16));
}

inline void pwm_set_pulse_len(uint8_t ch, uint16_t val) {
    uint32_t reg  = PWM_BASE + PWM_CH0 + ch * 4;
    uint32_t temp = read32(reg) & ~0xFFFF;
    write32(reg, temp | val);
}

inline void pwm_enable(uint8_t ch) {
    write32(PWM_BASE, (read32(PWM_BASE) | (1 << (6 + ch * 15))));
    write32(PWM_BASE, (read32(PWM_BASE) | (1 << (4 + ch * 15))));
}

inline void pwm_disable(uint8_t ch) {
    write32(PWM_BASE, (read32(PWM_BASE) & ~(1 << (4 + ch * 15))));
    write32(PWM_BASE, (read32(PWM_BASE) & ~(1 << (6 + ch * 15))));
}

inline void pwm_pulse_start(uint8_t ch) {
    write32(PWM_BASE, (read32(PWM_BASE) | (1 << (8 + ch * 15))));
}
