#pragma once

#include <stdint.h>

typedef struct {
    const uint8_t* data;
    uint8_t char_w;
    uint8_t char_h;
    uint8_t type;
    uint8_t offset;
} lcd_font_t;

extern const lcd_font_t clock_digits;
extern const lcd_font_t clock_dots;
extern const lcd_font_t t_12x24_full;
extern const lcd_font_t t_8x16_full;
