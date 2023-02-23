
#ifndef __F1C100S_TOUCH_H__
#define __F1C100S_TOUCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define F1C100S_TP_BASE (0x01C24800)

typedef enum
{
    TP_CTRL0            = 0x00,
    TP_CTRL1            = 0x04,
    TP_CTRL2            = 0x08,
    TP_CTRL3            = 0x0C,
    TP_INT_FIFO_CTRL    = 0x10,
    TP_INT_FIFO_STAT    = 0x14,
    TP_COM_DATA         = 0x1C,
    TP_DATA             = 0x24,
} tp_reg_e;

typedef enum
{
    TP_INT_OVERRUN      = (1 << 17),
    TP_INT_FIFO_DATA    = (1 << 16),
    TP_INT_UP           = (1 << 1),
    TP_INT_DOWN         = (1 << 0),
} tp_int_e;



#ifdef __cplusplus
}
#endif

#endif /* __F1C100S_TOUCH_H__ */
