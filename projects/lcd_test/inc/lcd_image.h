#pragma once

#include <stdint.h>

typedef struct {
    const uint8_t* data;
    uint16_t width;
    uint16_t height;
} tImage;

extern const tImage Logo;
