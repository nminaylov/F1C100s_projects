#include <stdio.h>
#include <string.h>
#include <math.h>
#include "system.h"
#include "f1c100s_de.h"
#include "f1c100s_timer.h"
#include "f1c100s_intc.h"
#include "f1c100s_gpio.h"
#include "f1c100s_pwm.h"
#include "arm32.h"

/*
 * Simple demonstaration project. Controls 2 LEDs:
 * LED1(GPIOA1): Classical, arduino-style LED blinking with delay function.
 * LED2(GPIOA2): "breathing" effect, created by sinewave-modulated PWM. Uses timer interrupt to update PWM duty
 *
 */

void delay(uint32_t t);
void timer_init(void);
void timer_irq_handler(void);

#define LED_FADE_STEPS 1000
#define LED_PWM_PERIOD 200

volatile uint8_t tick_flag = 0;
uint16_t sin_values[LED_FADE_STEPS / 2];

int main(void) {
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    printf("Hello, world!\r\n");

    // LED1 GPIO initialization
    gpio_pin_init(GPIOA, 1, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, GPIO_DRV_3);

    // LED2 GPIO + PWM initialization
    gpio_pin_init(GPIOA, 2, GPIO_MODE_AF3, GPIO_PULL_NONE, GPIO_DRV_3);
    pwm_init(PWM0, PWM_MODE_CONTINUOUS, 1, PWM_PSC_120); // 24M / 120 = 200kHz
    pwm_set_period(PWM0, LED_PWM_PERIOD); // 200k / 200 = 1kHz
    pwm_set_pulse_len(PWM0, 0);
    pwm_enable(PWM0);

    // Create a table with half-period of sin. Just to make sure math functions are working
    for(uint16_t i = 0; i < LED_FADE_STEPS / 2; i++) {
        float arg = M_PI / (float)(LED_FADE_STEPS / 2) * (float)i - (M_PI / 2.f); // -pi/2 .. pi/2
        float val = sinf(arg); // -1 .. 1
        sin_values[i] = (uint16_t)((val / 2.f + 0.5f) * (float)LED_PWM_PERIOD);
    }

    timer_init();

    while(1) {
        gpio_pin_set(GPIOA, 1);
        delay(200);
        gpio_pin_clear(GPIOA, 1);
        delay(200);
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

void timer_irq_handler(void) {
    static uint16_t step = 0;
    static uint8_t dir   = 0;
    tick_flag            = 1;
    if(dir == 0) { // First half of sinewave
        step++;
        if(step == LED_FADE_STEPS / 2 - 1) dir = 1;
    } else { // Second half of sinewave
        step--;
        if(step == 0) dir = 0;
    }
    // Update PWM pulse length
    pwm_set_pulse_len(PWM0, sin_values[step]);
    tim_clear_irq(TIM0);
}

void delay(uint32_t t) {
    while(1) {
        if(t == 0) return;
        if(tick_flag == 1) {
            tick_flag = 0;
            t--;
        }
    }
}
