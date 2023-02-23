#ifndef __LCD_H
#define __LCD_H

#include <stdint.h>
#include "lcd_image.h"
#include "lcd_font.h"
#include "printf.h"

#define LCD_W 720
#define LCD_H 480

#define SAVE_X_OFFSET

#define RED   0xff0000//0xF800
#define GREEN 0x00ff00//0x07E0
#define BLUE  0x0000ff//0x001E
#define BLACK 0x000000//0x0000
#define WHITE 0xffffff//0xFFFF

//#define BLACK           0x0000      /*   0,   0,   0 */
//#define NAVY            0x000F      /*   0,   0, 128 */
//#define DGREEN          0x03E0      /*   0, 128,   0 */
//#define DCYAN           0x03EF      /*   0, 128, 128 */
//#define MAROON          0x7800      /* 128,   0,   0 */
//#define PURPLE          0x780F      /* 128,   0, 128 */
//#define OLIVE           0x7BE0      /* 128, 128,   0 */
//#define LGRAY           0xC618      /* 192, 192, 192 */
//#define DGRAY           0x7BEF      /* 128, 128, 128 */
//#define BLUE            0x001F      /*   0,   0, 255 */
//#define GREEN           0x07E0      /*   0, 255,   0 */
//#define CYAN            0x07FF      /*   0, 255, 255 */
//#define RED             0xF800      /* 255,   0,   0 */
//#define MAGENTA         0xF81F      /* 255,   0, 255 */
//#define YELLOW          0xFFE0      /* 255, 255,   0 */
//#define WHITE           0xFFFF      /* 255, 255, 255 */
//#define ORANGE          0xFD20      /* 255, 165,   0 */
//#define GREENYELLOW     0xAFE5      /* 173, 255,  47 */
//#define BROWN           0xBC40
//#define BRRED           0xFC07

#define LCD_printf(...) fctprintf(&LCD_out, NULL, __VA_ARGS__)

void LCD_Init(uint8_t layer);
void LCD_SetPixel(uint16_t x, uint16_t y, uint32_t color);
void LCD_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void LCD_putchar(char chr);
void LCD_Image_mono(tImage img, uint16_t x, uint16_t y);
void LCD_print(char * str);
void LCD_out(char character, void* arg);
void LCD_SetTextColor(uint32_t color);
void LCD_SetBGColor(uint32_t color);
void LCD_SetTextPos(uint16_t x, uint16_t y);
void LCD_SetFont(const tFont * fnt);



#endif
