#include "f1c100s_timer.h"
#include "io.h"

/************** General-purpose imers ***************/

void tim_init(uint8_t ch, tim_mode_e mode, tim_source_e src, tim_prescaller_e psc) {
    uint32_t val = (mode << 7) | (psc << 4) | (src << 2);
    write32(TIMER_BASE + TIM_0_CTRL + ch * 0x10, val);
}

void tim_set_period(uint8_t ch, uint32_t val) {
    write32(TIMER_BASE + TIM_0_INTV + ch * 0x10, val);
}

inline uint32_t tim_get_cnt(uint8_t ch) {
    return read32(TIMER_BASE + TIM_0_CUR + ch * 0x10);
}

inline void tim_set_cnt(uint8_t ch, uint32_t val) {
    write32(TIMER_BASE + TIM_0_CUR + ch * 0x10, val);
}

inline void tim_start(uint8_t ch) {
    uint32_t reg = TIMER_BASE + TIM_0_CTRL + ch * 0x10;
    write32(reg, (read32(reg) | (1 << 0) | (1 << 1)));
}

inline void tim_stop(uint8_t ch) {
    uint32_t reg = TIMER_BASE + TIM_0_CTRL + ch * 0x10;
    write32(reg, (read32(reg) & ~(1 << 0)));
}

inline void tim_reload(uint8_t ch) {
    uint32_t reg = TIMER_BASE + TIM_0_CTRL + ch * 0x10;
    write32(reg, (read32(reg) | (1 << 1)));
}

inline void tim_int_enable(uint8_t ch) {
    write32(TIMER_BASE + TIM_IRQ_EN, (read32(TIMER_BASE + TIM_IRQ_EN) | (1 << ch)));
}

inline void tim_int_disable(uint8_t ch) {
    write32(TIMER_BASE + TIM_IRQ_EN, (read32(TIMER_BASE + TIM_IRQ_EN) & ~(1 << ch)));
}

inline uint8_t tim_get_int_status(void) {
    return read32(TIMER_BASE + TIM_IRQ_STA) & 0x7;
}

inline void tim_clear_irq(uint8_t ch) {
    write32(TIMER_BASE + TIM_IRQ_STA, (1 << ch));
}

//  TODO: aws

/************** Watchdog timer ***************/

void wdg_init(wdg_mode_e mode, wdg_period_e period) {
    write32(TIMER_BASE + WDG_CFG, mode);
    write32(TIMER_BASE + WDG_MODE, (period << 4) | 1);

    wdg_feed();
}

void wdg_disable(void) {
    wdg_feed();
    write32(TIMER_BASE + WDG_MODE, 0);
    wdg_feed();
}

inline void wdg_feed(void) {
    write32(TIMER_BASE + WDG_CTRL, (0xA57 << 1) | 1);
}

inline uint8_t wdg_get_int_status(void) {
    return read32(TIMER_BASE + WDG_IRQ_STA) & 0x1;
}
