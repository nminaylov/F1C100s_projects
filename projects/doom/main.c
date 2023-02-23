#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "io.h"
#include "display.h"
#include "arm32.h"
#include "f1c100s_gpio.h"
#include "f1c100s_clock.h"
#include "f1c100s_de.h"
#include "f1c100s_timer.h"
#include "f1c100s_intc.h"
#include "ff.h"

void timer_init(void);
void timer_irq_handler(void);
extern void D_DoomMain(void);

int main(void) {
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    display_init();
    display_set_bl(100);

    gpio_pin_init(GPIOA, 3, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, GPIO_DRV_3);
    gpio_pin_set(GPIOA, 3);

    timer_init();

    FATFS fs;
    uint8_t state = f_mount(&fs, "", 1);
    printf("Mount: %d\r\n", state);
    D_DoomMain();

    while(1) {
    }
    return 0;
}

void timer_init(void) {
    // Configure timer to generate update event every 1ms
    tim_init(TIM0, TIM_MODE_CONT, TIM_SRC_HOSC, TIM_PSC_1);
    tim_set_period(TIM0, 24000000UL / 1000UL);
    tim_int_enable(TIM0);
    // IRQ configuration
    intc_set_irq_handler(IRQ_TIMER0, timer_irq_handler);
    intc_enable_irq(IRQ_TIMER0);

    tim_start(TIM0);
}

volatile uint32_t systime = 0;

void timer_irq_handler(void) {
    systime++;
    tim_clear_irq(TIM0);
}

void sleep_ms(uint32_t ms) {
}
