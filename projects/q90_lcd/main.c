#include <stdio.h>
#include <string.h>
#include <math.h>
#include "system.h"
#include "f1c100s_clock.h"
#include "arm32.h"
#include "display.h"
#include "lcd.h"
#include "q90_buttons.h"

int main(void) {
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    printf("Hello, world!\r\n");

    display_init();
    display_set_bl(100);

    lcd_init(0);

    lcd_set_font(&t_8x16_full);
    lcd_printf("CPU:  %ld\n", clk_cpu_get_freq());
    lcd_printf("HCLK: %ld\n", clk_hclk_get_freq());
    lcd_printf("AHB:  %ld\n", clk_ahb_get_freq());
    lcd_printf("APB:  %ld\n\n", clk_apb_get_freq());
    lcd_printf("PLL_CPU:    %ld\n", clk_pll_get_freq(PLL_CPU));
    lcd_printf("PLL_AUDIO:  %ld\n", clk_pll_get_freq(PLL_AUDIO));
    lcd_printf("PLL_VIDEO:  %ld\n", clk_pll_get_freq(PLL_VIDEO));
    lcd_printf("PLL_VE:     %ld\n", clk_pll_get_freq(PLL_VE));
    lcd_printf("PLL_DDR:    %ld\n", clk_pll_get_freq(PLL_DDR));
    lcd_printf("PLL_PERIPH: %ld\n", clk_pll_get_freq(PLL_PERIPH));

    for(uint8_t i = 0; i < 255; i++) {
        lcd_fill(32 + i, 200, 1, 10, COLOR_BLACK | (i << 16));
        lcd_fill(32 + i, 210, 1, 10, COLOR_BLACK | (i << 8));
        lcd_fill(32 + i, 220, 1, 10, COLOR_BLACK | (i));
        lcd_fill(32 + i, 230, 1, 10, COLOR_BLACK | (i << 16) | (i << 8) | (i));
    }

    q90_buttons_init();

    while(1) {
        uint32_t btn_mask = q90_buttons_read();

        lcd_set_text_pos(220, 0);
        lcd_printf("UP:    %u\n", (btn_mask & BTN_UP) != 0);
        lcd_printf("DOWN:  %u\n", (btn_mask & BTN_DONW) != 0);
        lcd_printf("LEFT:  %u\n", (btn_mask & BTN_LEFT) != 0);
        lcd_printf("RIGHT: %u\n", (btn_mask & BTN_RIGHT) != 0);

        lcd_printf("A: %u\n", (btn_mask & BTN_A) != 0);
        lcd_printf("B: %u\n", (btn_mask & BTN_B) != 0);
        lcd_printf("X: %u\n", (btn_mask & BTN_X) != 0);
        lcd_printf("Y: %u\n", (btn_mask & BTN_Y) != 0);
        lcd_printf("L: %u\n", (btn_mask & BTN_L) != 0);
        lcd_printf("R: %u\n", (btn_mask & BTN_R) != 0);

        lcd_printf("START:  %u\n", (btn_mask & BTN_START) != 0);
        lcd_printf("SELECT: %u\n", (btn_mask & BTN_SELECT) != 0);

        lcd_set_text_pos(270, 100);
        lcd_printf("+: %u\n", (btn_mask & BTN_PLUS) != 0);
        lcd_printf("-: %u\n", (btn_mask & BTN_MINUS) != 0);
    }
    return 0;
}
