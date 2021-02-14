#ifndef __LCD_IMAGE
#define __LCD_IMAGE

#include <stdint.h>

typedef struct
{
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
}tImage;

extern const tImage Logo;

#endif /* __LCD_IMAGE */
