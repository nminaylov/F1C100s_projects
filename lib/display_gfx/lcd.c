#include "lcd.h"
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include "f1c100s_de.h"

#define SAVE_X_OFFSET

static uint32_t bg_color   = COLOR_BLACK;
static uint32_t text_color = COLOR_WHITE;

static uint16_t text_x       = 0;
static uint16_t text_y       = 0;
static uint16_t text_x_start = 0;

static lcd_font_t* font;

#define LCD_BPP 32

#if LCD_BPP == 32
static uint32_t lcd_fb[DISPLAY_W * DISPLAY_H];
#elif LCD_BPP == 16
static uint16_t lcd_fb[DISPLAY_W * DISPLAY_H];
#elif LCD_BPP == 8
static uint8_t lcd_fb[DISPLAY_W * DISPLAY_H];
#else

#endif

void lcd_init(uint8_t layer) {
    debe_layer_init(layer);
    debe_layer_set_size(layer, DISPLAY_W, DISPLAY_H);
    debe_layer_set_addr(layer, lcd_fb);
    debe_layer_enable(layer);

#if LCD_BPP == 32
    debe_layer_set_mode(layer, DEBE_MODE_32BPP_ARGB_8888);
#elif LCD_BPP == 16
    debe_layer_set_mode(layer, DEBE_MODE_16BPP_RGB_565);
#elif LCD_BPP == 8
    debe_layer_set_mode(layer, DEBE_MODE_8BPP_MONO);
#endif

    lcd_fill(0, 0, DISPLAY_W, DISPLAY_H, COLOR_BLACK);
    lcd_set_font(&t_12x24_full);
}

inline void lcd_set_pixel(uint16_t x, uint16_t y, uint32_t color) {
#if LCD_BPP == 32
    lcd_fb[x + y * DISPLAY_W] = color;
#elif LCD_BPP == 16
    lcd_fb[x + y * DISPLAY_W] = color & 0xFFFF;
#elif LCD_BPP == 8
    lcd_fb[x + y * DISPLAY_W] = color & 0xFF;
#else

#endif
}

void lcd_fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
    uint32_t offset = 0;
    if((x + w > DISPLAY_W) || (y + h > DISPLAY_H)) return;
    for(uint16_t ypos = y; ypos < (y + h); ypos++) {
        offset = (uint32_t)ypos * DISPLAY_W;
        for(uint16_t xpos = x; xpos < (x + w); xpos++) {
            lcd_fb[offset + xpos] = color;
        }
    }
}

static void lcd_draw_char(uint16_t x, uint16_t y, uint8_t* start) {
    uint16_t xpos = 0;
    uint16_t ypos = 0;
    if(font->type == 0) {
        uint16_t byte_num = font->char_w * font->char_h / 8;
        for(uint16_t byte_cnt = 0; byte_cnt < byte_num; byte_cnt++) {
            for(uint8_t bit_cnt = 0; bit_cnt < 8; bit_cnt++) {
                if((start[byte_cnt] << bit_cnt) & 0x80)
                    lcd_set_pixel(x + xpos, y + ypos, text_color);
                else
                    lcd_set_pixel(x + xpos, y + ypos, bg_color);

                if((++xpos) >= font->char_w) {
                    xpos = 0;
                    ypos++;
                }
            }
        }
    }
}

void lcd_putchar(char chr) {
    uint16_t cur_X = text_x;
    uint16_t cur_Y = text_y;
    if(chr == '\n') {
#ifdef SAVE_X_OFFSET
        text_x = text_x_start;
#else
        text_x = 0;
#endif
        text_y += font->char_h;
        chr = 0;
    } else {
        text_x += font->char_w;
        if(text_x > (DISPLAY_W - font->char_w)) {
#ifdef SAVE_X_OFFSET
            text_x = text_x_start;
#else
            text_x = 0;
#endif
            text_y += font->char_h;
        }
    }
    if(text_y > (DISPLAY_H - font->char_h)) text_y = 0;

    if(chr < font->offset) chr = font->offset;

    uint8_t* chardata = NULL;

    if(font->type == 0) {
        if(chr < font->offset) chr = font->offset;
        chardata =
            (uint8_t*)&(font->data[(font->char_w * font->char_h / 8) * (chr - font->offset)]);
    }

    lcd_draw_char(cur_X, cur_Y, chardata);
}

void lcd_print(char* str) {
    //text_x_start = text_x;
    while(*str) lcd_putchar(*str++);
}

inline void lcd_out(char character, void* arg) {
    lcd_putchar(character);
}

inline void lcd_set_text_color(uint32_t color) {
    text_color = color;
}

inline void lcd_set_bg_color(uint32_t color) {
    bg_color = color;
}

inline void lcd_set_text_pos(uint16_t x, uint16_t y) {
    text_x       = x;
    text_y       = y;
    text_x_start = text_x;
}

inline void lcd_set_font(const lcd_font_t* fnt) {
    font = (lcd_font_t*)fnt;
}
