#ifndef __LCD_FONT_H
#define __LCD_FONT_H

#include <stdint.h>

typedef struct
{
    const uint8_t *data;
    uint8_t char_w;
    uint8_t char_h;
    uint8_t type;
    uint8_t offset;
}tFont;

extern const tFont clock_digits;
extern const tFont clock_dots;
extern const tFont t_12x24_full;
extern const tFont page_markers;
extern const tFont t_8x14_rus;
extern const tFont t_16x22_digits;
extern const tFont symbols;


#endif
