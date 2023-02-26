#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "io.h"
#include "display.h"
#include "lcd.h"
#include "arm32.h"

int main(void) {
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    printf("Hello from firmware!\r\n");

    display_init();
    display_set_bl(100);
    lcd_init(0);

    lcd_printf("Hello, world!");

    lcd_fill(50, 40, 100, 100, COLOR_RED);
    lcd_fill(50, 140, 100, 100, COLOR_GREEN);
    lcd_fill(50, 240, 100, 100, COLOR_BLUE);
    lcd_fill(50, 340, 100, 100, COLOR_WHITE);

    lcd_fill(196, 46, 160, 58, COLOR_BLUE);
    lcd_set_bg_color(COLOR_BLUE);
    lcd_set_text_color(COLOR_WHITE);
    lcd_set_text_pos(200, 50);

    lcd_set_font(&clock_digits);
    lcd_printf("%02d", 12);

    lcd_set_font(&clock_dots);
    //    if (time.sec%2)
    lcd_putchar(1);
    //    else
    //        lcd_putchar(0);

    lcd_set_font(&clock_digits);
    lcd_printf("%02d", 34);

    lcd_set_font(&t_12x24_full);
    lcd_set_bg_color(COLOR_BLACK);
    lcd_set_text_color(COLOR_WHITE);
    lcd_set_text_pos(200, 200);
    for(uint16_t ch = 0; ch <= 0xFF; ch++) {
        lcd_putchar(ch & 0xFF);
        if(ch % 32 == 0) lcd_putchar('\n');
    }

    while(1) {
    }

    return 0;
}
