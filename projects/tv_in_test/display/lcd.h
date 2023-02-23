#pragma once

#include <stdint.h>
#include "lcd_font.h"
#include "printf.h"

#define LCD_W 800
#define LCD_H 480

#define SAVE_X_OFFSET

#define RED 0xff0000 //0xF800
#define GREEN 0x00ff00 //0x07E0
#define BLUE 0x0000ff //0x001E
#define BLACK 0x000000 //0x0000
#define WHITE 0xffffff //0xFFFF

#define LCD_printf(...) fctprintf(&LCD_out, NULL, __VA_ARGS__)

void LCD_Init(uint8_t layer);
void LCD_SetPixel(uint16_t x, uint16_t y, uint32_t color);
void LCD_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void LCD_putchar(char chr);
void LCD_print(char* str);
void LCD_out(char character, void* arg);
void LCD_SetTextColor(uint32_t color);
void LCD_SetBGColor(uint32_t color);
void LCD_SetTextPos(uint16_t x, uint16_t y);
void LCD_SetFont(const tFont* fnt);
