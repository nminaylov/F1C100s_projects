#pragma once

#include <stdint.h>
#include "lcd_font.h"
#include "printf.h"

#define COLOR_RED 0xFFFF0000
#define COLOR_GREEN 0xFF00FF00
#define COLOR_BLUE 0xFF0000FF
#define COLOR_BLACK 0xFF000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_TRANSPARENT 0x00000000

#define lcd_printf(...) fctprintf(&lcd_out, NULL, __VA_ARGS__)

void lcd_init(uint8_t layer);

void lcd_set_pixel(uint16_t x, uint16_t y, uint32_t color);

void lcd_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);

void lcd_putchar(char chr);

void lcd_print(char* str);

void lcd_out(char character, void* arg);

void lcd_set_text_color(uint32_t color);

void lcd_set_bg_color(uint32_t color);

void lcd_set_text_pos(uint16_t x, uint16_t y);

void lcd_set_font(const lcd_font_t* fnt);
