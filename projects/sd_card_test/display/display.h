#pragma once

#include <stdint.h>

void display_init(void);
void display_set_bl(uint8_t duty);

#define GFX_RGB565(r, g, b) \
    ((((r & 0xF8) >> 3) << 11) | (((g & 0xFC) >> 2) << 5) | ((b & 0xF8) >> 3))

#define GFX_RGB565_R(color) ((0xF800 & color) >> 11)
#define GFX_RGB565_G(color) ((0x07E0 & color) >> 5)
#define GFX_RGB565_B(color) (0x001F & color)

#define GFX_ARGB8888(r, g, b, a) \
    (((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF))

#define GFX_ARGB8888_R(color) ((color & 0x00FF0000) >> 16)
#define GFX_ARGB8888_G(color) ((color & 0x0000FF00) >> 8)
#define GFX_ARGB8888_B(color) ((color & 0x000000FF))
#define GFX_ARGB8888_A(color) ((color & 0xFF000000) >> 24)
