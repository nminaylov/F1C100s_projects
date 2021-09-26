#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "system.h"
#include "io.h"
#include "lcd.h"
#include "display.h"
#include "arm32.h"
#include "f1c100s_clock.h"
#include "f1c100s_de.h"
#include "f1c100s_tve.h"
#include "f1c100s_tvd.h"
#include "f1c100s_intc.h"

static uint8_t fb_y[720*576];
static uint8_t fb_c[720*576];

int main(void)
{
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    display_init();
    display_set_bl(100);

//    de_tv_init(TVE_MODE_NTSC, 720);

    debe_set_bg_color(0x0000FF00);
    debe_load(DEBE_UPDATE_AUTO);


    uint16_t tv_w, tv_h;
    tvd_init(TVD_MODE_NTSC, fb_y, fb_c, 0);
    tvd_set_out_fmt(TVD_FMT_422_PL);
    tvd_get_out_size(&tv_w, &tv_h);
    tvd_set_bluescreen_mode(TVD_BLUE_OFF);

    defe_init_spl_422(tv_w, tv_h, fb_y, fb_c);

    debe_layer_init(0); // Layer 0 - video
    debe_layer_set_size(0, tv_w, tv_h);
    debe_layer_set_mode(0, DEBE_MODE_DEFE_VIDEO);
    debe_layer_enable(0);

    tvd_enable();

    LCD_Init(1); // Layer 1 - overlay
    LCD_SetBGColor(0x80000000);
    LCD_SetTextPos(200, 0);
    LCD_printf("NTSC");

//    LCD_Fill(700, 460, 10, 10, 0xFFFF0000);

//    for (uint8_t i = 0; i < 255; i++)
//    {
//        LCD_Fill(200+i, 50, 1, 40, (i << 24)|(0xFF0000));
//        LCD_Fill(200+i, 100, 1, 40, (i << 24)|(0x00FF00));
//        LCD_Fill(200+i, 150, 1, 40, (i << 24)|(0x0000FF));
//        LCD_Fill(200+i, 200, 1, 40, (i << 24)|(0xFFFFFF));
//        LCD_Fill(200+i, 250, 1, 40, (i << 24)|(0x000000));
//    }

    while (1)
    {
        LCD_SetTextPos(0, 0);
        uint32_t val = read32(F1C100S_TVD_BASE+TVD_STATE_0);
        LCD_printf("%08X\n", val);
        val = read32(F1C100S_TVD_BASE+TVD_STATE_1);
        LCD_printf("%08X\n", val);
        val = read32(F1C100S_TVD_BASE+TVD_STATE_2);
        LCD_printf("%08X\n", val);
        val = read32(F1C100S_TVD_BASE+TVD_STATE_3);
        LCD_printf("%08X\n", val);
        val = read32(F1C100S_TVD_BASE+TVD_STATE_4);
        LCD_printf("%08X\n", val);
    }
    return 0;
}
