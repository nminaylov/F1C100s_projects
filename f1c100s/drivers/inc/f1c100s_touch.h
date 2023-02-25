#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    TP_CTRL0         = 0x00,
    TP_CTRL1         = 0x04,
    TP_CTRL2         = 0x08,
    TP_CTRL3         = 0x0C,
    TP_INT_FIFO_CTRL = 0x10,
    TP_INT_FIFO_STAT = 0x14,
    TP_COM_DATA      = 0x1C,
    TP_DATA          = 0x24,
} tp_reg_e;

typedef enum {
    TP_INT_OVERRUN   = (1 << 17),
    TP_INT_FIFO_DATA = (1 << 16),
    TP_INT_UP        = (1 << 1),
    TP_INT_DOWN      = (1 << 0),
} tp_int_e;

void tp_init(void);

void tp_int_config(uint32_t int_mask);

uint32_t tp_int_get_state(void);

void tp_int_clear(uint32_t int_mask);

void tp_fifo_flush(void);

void tp_fifo_set_trig_level(uint8_t lvl);

void tp_fifo_read(uint16_t* data, uint8_t len);

#ifdef __cplusplus
}
#endif
