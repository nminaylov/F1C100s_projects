#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "f1c100s_timer.h"
#include "f1c100s_intc.h"
#include "io.h"
#include "display.h"
#include "arm32.h"
#include "lvgl.h"
#include "lv_demo_widgets.h"

void timer_init(void);
void timer_irq_handler(void);

void lvgl_print_cb(lv_log_level_t level, const char *file, uint32_t line, const char *func, const char *buf);

int main(void)
{
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    printf("Hello from firmware!\r\n");
    timer_init();

    lv_init();
    lv_log_register_print_cb(lvgl_print_cb);
    display_init_lvgl();
    display_set_bl(100);
    //touch_init_lvgl();

    lv_demo_widgets();

    while (1)
    {
        lv_task_handler();
    }
    return 0;
}

void timer_init(void)
{
    // Configure timer to generate update event every 1ms
    tim_init(TIM0, TIM_MODE_CONT, TIM_SRC_HOSC, TIM_PSC_1);
    tim_set_period(TIM0, 24000000UL / 1000UL);
    tim_int_enable(TIM0);
    // IRQ configuration
    intc_set_irq_handler(IRQ_TIMER0, timer_irq_handler);
    intc_enable_irq(IRQ_TIMER0);

    tim_start(TIM0);
}

void timer_irq_handler(void)
{
    lv_tick_inc(1);
    tim_clear_irq(TIM0);
}

void lvgl_print_cb(lv_log_level_t level, const char *file, uint32_t line, const char *func, const char *buf)
{
    /*Use only the file name not the path*/
    size_t p;
    for (p = strlen(file); p > 0; p--)
    {
        if (file[p] == '/' || file[p] == '\\')
        {
            p++; /*Skip the slash*/
            break;
        }
    }

    static const char *lvl_prefix[] =
        { "Trace", "Info", "Warn", "Error", "User" };
    printf("%s: %s \t(%s #%lu %s())\r\n", lvl_prefix[level], buf, &file[p], line, func);
}

