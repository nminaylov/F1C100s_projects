#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

typedef enum {
    INTC_VECTOR    = 0x00,
    INTC_BASE_ADDR = 0x04,
    INTC_NMI_CTRL  = 0x0C,
    INTC_PEND0     = 0x10,
    INTC_PEND1     = 0x14,
    INTC_ENABLE0   = 0x20,
    INTC_ENABLE1   = 0x24,
    INTC_MASK0     = 0x30,
    INTC_MASK1     = 0x34,
    INTC_RESP0     = 0x40,
    INTC_RESP1     = 0x44,
    INTC_FORCE0    = 0x50,
    INTC_FORCE1    = 0x54,
    INTC_PRIORITY0 = 0x60,
    INTC_PRIORITY1 = 0x64,
    INTC_PRIORITY2 = 0x68,
    INTC_PRIORITY3 = 0x6C,
} intc_reg_e;

typedef enum {
    IRQ_NMI   = 0,
    IRQ_UART0 = 1,
    IRQ_UART1 = 2,
    IRQ_UART2 = 3,

    IRQ_OWA  = 5,
    IRQ_CIR  = 6,
    IRQ_I2C0 = 7,
    IRQ_I2C1 = 8,
    IRQ_I2C2 = 9,
    IRQ_SPI0 = 10,
    IRQ_SPI1 = 11,

    IRQ_TIMER0 = 13,
    IRQ_TIMER1 = 14,
    IRQ_TIMER2 = 15,
    IRQ_WDOG   = 16,
    IRQ_RSB    = 17,
    IRQ_DMA    = 18,

    IRQ_TP     = 20,
    IRQ_AUDIO  = 21,
    IRQ_KEYADC = 22,
    IRQ_MMC0   = 23,
    IRQ_MMC1   = 24,

    IRQ_USBOTG = 26,
    IRQ_TVD    = 27,
    IRQ_TVE    = 28,
    IRQ_TCON   = 29,
    IRQ_DEFE   = 30,
    IRQ_DEBE   = 31,
    IRQ_CSI    = 32,
    IRQ_DEITLA = 33,
    IRQ_VE     = 34,
    IRQ_I2S    = 35,

    IRQ_GPIOD = 38,
    IRQ_GPIOE = 39,
    IRQ_GPIOF = 40,
} intc_irq_vector_e;

typedef void (*intc_irq_handler)(void);

void intc_init(void);

void intc_enable_irq(intc_irq_vector_e irq);

void intc_disable_irq(intc_irq_vector_e irq);

void intc_set_priority(intc_irq_vector_e irq, uint8_t prio);

void intc_set_irq_handler(intc_irq_vector_e irq, intc_irq_handler handler);

#ifdef __cplusplus
}
#endif
