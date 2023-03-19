#pragma once
#include <stdint.h>

typedef enum {
    BTN_UP     = (1 << 0),
    BTN_DONW   = (1 << 1),
    BTN_LEFT   = (1 << 2),
    BTN_RIGHT  = (1 << 3),
    BTN_A      = (1 << 4),
    BTN_B      = (1 << 5),
    BTN_X      = (1 << 6),
    BTN_Y      = (1 << 7),
    BTN_L      = (1 << 8),
    BTN_R      = (1 << 9),
    BTN_START  = (1 << 10),
    BTN_SELECT = (1 << 11),
    BTN_PLUS   = (1 << 12),
    BTN_MINUS  = (1 << 13),
} q90_btn_e;

void q90_buttons_init(void);

uint32_t q90_buttons_read(void);
