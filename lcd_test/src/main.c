#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "io.h"
#include "display.h"
#include "lcd.h"
#include "arm32.h"


int main(void)
{
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    printf("Hello from firmware!\r\n");

    LCD_Init();
    display_set_bl(100);

    LCD_printf("Hello, world!");

    LCD_Fill(50, 40, 100, 100, RED);
    LCD_Fill(50, 140, 100, 100, GREEN);
    LCD_Fill(50, 240, 100, 100, BLUE);
    LCD_Fill(50, 340, 100, 100, WHITE);

    LCD_Fill(196, 46, 160, 58, BLUE);
    LCD_SetBGColor(BLUE);
    LCD_SetTextColor(WHITE);
    LCD_SetTextPos(200,50);

    LCD_SetFont(&clock_digits);
    LCD_printf("%02d",12);

    LCD_SetFont(&clock_dots);
//    if (time.sec%2)
        LCD_putchar(1);
//    else
//        LCD_putchar(0);

    LCD_SetFont(&clock_digits);
    LCD_printf("%02d",34);

    LCD_SetFont(&t_12x24_full);
    LCD_SetBGColor(BLACK);
    LCD_SetTextColor(WHITE);
    LCD_SetTextPos(200, 200);
    for (uint16_t ch = 0; ch <= 0xFF; ch++)
    {
        LCD_putchar(ch&0xFF);
        if (ch%32 == 0)
            LCD_putchar('\n');
    }

    while (1)
    {

    }
    return 0;
}

