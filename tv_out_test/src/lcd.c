#include "lcd.h"
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include "f1c100s_de.h"

uint32_t BGColor = 0; //WHITE;
uint32_t TextColor = 0; //BLACK;

uint16_t Text_X = 0;
uint16_t Text_Y = 0;
uint16_t Text_X_start = 0;

tFont *Font;

#define LCD_BPP 32

#if LCD_BPP == 32
uint32_t LCD_FB[LCD_W * LCD_H];
#elif LCD_BPP == 16
uint16_t LCD_FB[LCD_W*LCD_H];
#elif LCD_BPP == 8
uint8_t LCD_FB[LCD_W*LCD_H];
#else

#endif

void LCD_Init(uint8_t layer)
{
    //display_init();
    debe_layer_init(layer);
    debe_layer_set_size(layer, LCD_W, LCD_H);
    debe_layer_set_addr(layer, LCD_FB);
    debe_layer_enable(layer);

#if LCD_BPP == 32
    debe_layer_set_mode(layer, DEBE_MODE_32BPP_RGB_888);
#elif LCD_BPP == 16
    debe_layer_set_mode(layer, DEBE_MODE_16BPP_RGB_565);
#elif LCD_BPP == 8
    debe_layer_set_mode(layer, DEBE_MODE__8BPP_MONO);
#endif

    LCD_Fill(0, 0, 800, 480, BLACK);

    LCD_SetFont(&t_12x24_full);
    LCD_SetBGColor(BLACK);
    LCD_SetTextColor(WHITE);
}

static void LCD_DrawChar(uint16_t x, uint16_t y, uint8_t *start);

inline void LCD_SetPixel(uint16_t x, uint16_t y, uint32_t color)
{

#if LCD_BPP == 32
    LCD_FB[x + y * LCD_W] = color;
#elif LCD_BPP == 16
    LCD_FB[x+y*LCD_W] = color & 0xFFFF;
#elif LCD_BPP == 8
    LCD_FB[x+y*LCD_W] = color & 0xFF;
#else

#endif
}

void LCD_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color)
{
    uint32_t offset = 0;
    if ((x + w > LCD_W) || (y + h > LCD_H))
        return;
    for (uint16_t ypos = y; ypos < (y + h); ypos++)
    {
        offset = (uint32_t) ypos * LCD_W;
        for (uint16_t xpos = x; xpos < (x + w); xpos++)
        {
            LCD_FB[offset + xpos] = color;
        }
    }
}

void LCD_putchar(char chr)
{
    uint16_t cur_X = Text_X;
    uint16_t cur_Y = Text_Y;
    if (chr == '\n')
    {
#ifdef  SAVE_X_OFFSET
        Text_X = Text_X_start;
#else
        Text_X = 0;
#endif
        Text_Y += Font->char_h;
        chr = 0;
    }
    else
    {
        Text_X += Font->char_w;
        if (Text_X > (LCD_W - Font->char_w))
        {
#ifdef  SAVE_X_OFFSET
            Text_X = Text_X_start;
#else
            Text_X = 0;
#endif
            Text_Y += Font->char_h;
        }
    }
    if (Text_Y > (LCD_H - Font->char_h))
        Text_Y = 0;

    if (chr < Font->offset)
        chr = Font->offset;

    uint8_t *chardata = NULL;

    if (Font->type == 0)
    {
        if (chr < Font->offset)
            chr = Font->offset;
        chardata = (uint8_t*) &(Font->data[(Font->char_w * Font->char_h / 8) * (chr - Font->offset)]);
    }

    LCD_DrawChar(cur_X, cur_Y, chardata);
}

static void LCD_DrawChar(uint16_t x, uint16_t y, uint8_t *start)
{
    uint16_t xpos = 0;
    uint16_t ypos = 0;
    if (Font->type == 0)
    {
        uint16_t byte_num = Font->char_w * Font->char_h / 8;
        for (uint16_t byte_cnt = 0; byte_cnt < byte_num; byte_cnt++)
        {
            for (uint8_t bit_cnt = 0; bit_cnt < 8; bit_cnt++)
            {
                if ((start[byte_cnt] << bit_cnt) & 0x80)
                    LCD_SetPixel(x + xpos, y + ypos, TextColor);
                else
                    LCD_SetPixel(x + xpos, y + ypos, BGColor);

                if ((++xpos) >= Font->char_w)
                {
                    xpos = 0;
                    ypos++;
                }
            }
        }
    }
}

//void LCD_Image_mono(tImage img, uint16_t x, uint16_t y)
//{
//    LCD_SetWindow(x,y,img.width,img.height);
//    LCD_DC_UP;
//    LCD_CS_DN;
//    for (uint16_t byte_cnt = 0; byte_cnt < img.width*img.width/8; byte_cnt++)
//    {
//        for (uint8_t bit_cnt = 0; bit_cnt < 8; bit_cnt++)
//        {
//            if ((img.data[byte_cnt] << bit_cnt) & 0x80)
//                LCD_SendPixel(TextColor);
//            else
//                LCD_SendPixel(BGColor);
//        }
//    }
//    while (LL_SPI_IsActiveFlag_BSY(SPI1));
//    LCD_CS_UP;
//}

void LCD_print(char *str)
{
    //Text_X_start = Text_X;
    while (*str)
        LCD_putchar(*str++);
}

inline void LCD_out(char character, void *arg)
{
    LCD_putchar(character);
}

inline void LCD_SetTextColor(uint32_t color)
{
    TextColor = color;
}

inline void LCD_SetBGColor(uint32_t color)
{
    BGColor = color;
}

inline void LCD_SetTextPos(uint16_t x, uint16_t y)
{
    Text_X = x;
    Text_Y = y;
    Text_X_start = Text_X;
}

inline void LCD_SetFont(const tFont *fnt)
{
    Font = (tFont*) fnt;
}

