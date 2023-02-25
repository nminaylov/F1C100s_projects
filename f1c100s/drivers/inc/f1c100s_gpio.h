#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

#define GPIOA (GPIO_BASE + 0 * 0x24)
#define GPIOB (GPIO_BASE + 1 * 0x24)
#define GPIOC (GPIO_BASE + 2 * 0x24)
#define GPIOD (GPIO_BASE + 3 * 0x24)
#define GPIOE (GPIO_BASE + 4 * 0x24)
#define GPIOF (GPIO_BASE + 5 * 0x24)

#define GPIOD_INT (GPIO_BASE + 0x200 + 0 * 0x20)
#define GPIOE_INT (GPIO_BASE + 0x200 + 1 * 0x20)
#define GPIOF_INT (GPIO_BASE + 0x200 + 2 * 0x20)

typedef enum {
    GPIO_CFG0 = 0x00,
    GPIO_CFG1 = 0x04,
    GPIO_CFG2 = 0x08,
    GPIO_CFG3 = 0x0C,
    GPIO_DATA = 0x10,
    GPIO_DRV0 = 0x14,
    GPIO_DRV1 = 0x18,
    GPIO_PUL0 = 0x1C,
    GPIO_PUL1 = 0x20,
} gpio_reg_e;

typedef enum {
    GPIO_INT_CFG0 = 0x00,
    GPIO_INT_CFG1 = 0x04,
    GPIO_INT_CFG2 = 0x08,
    GPIO_INT_CFG3 = 0x0C,
    GPIO_INT_CTRL = 0x10,
    GPIO_INT_STA  = 0x14,
    GPIO_INT_DEB  = 0x18,
} gpio_int_reg_e;

typedef enum {
    PIN0  = (1U << 0),
    PIN1  = (1U << 1),
    PIN2  = (1U << 2),
    PIN3  = (1U << 3),
    PIN4  = (1U << 4),
    PIN5  = (1U << 5),
    PIN6  = (1U << 6),
    PIN7  = (1U << 7),
    PIN8  = (1U << 8),
    PIN9  = (1U << 9),
    PIN10 = (1U << 10),
    PIN11 = (1U << 11),
    PIN12 = (1U << 12),
    PIN13 = (1U << 13),
    PIN14 = (1U << 14),
    PIN15 = (1U << 15),
    PIN16 = (1U << 16),
    PIN17 = (1U << 17),
    PIN18 = (1U << 18),
    PIN19 = (1U << 19),
    PIN20 = (1U << 20),
    PIN21 = (1U << 21),
} gpio_pin_e;

typedef enum {
    GPIO_MODE_INPUT    = 0,
    GPIO_MODE_OUTPUT   = 1,
    GPIO_MODE_AF2      = 2,
    GPIO_MODE_AF3      = 3,
    GPIO_MODE_AF4      = 4,
    GPIO_MODE_AF5      = 5,
    GPIO_MODE_AF6      = 6,
    GPIO_MODE_DISABLED = 7,
} gpio_mode_e;

typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP   = 1,
    GPIO_PULL_DOWN = 2,
} gpio_pull_e;

typedef enum {
    GPIO_DRV_0 = 0,
    GPIO_DRV_1 = 1,
    GPIO_DRV_2 = 2,
    GPIO_DRV_3 = 3,
} gpio_drv_e;

typedef enum {
    EINT_TRG_RISING  = 0,
    EINT_TRG_FALLING = 1,
    EINT_TRG_HIGH    = 2,
    EINT_TRG_LOW     = 3,
    EINT_TRG_DOUBLE  = 4,
} eint_trigger_mode_e;

typedef enum {
    EINT_DEB_SRC_LOSC = 0,
    EINT_DEB_SRC_HOSC = 1,
} eint_debounce_src_e;

typedef enum {
    EINT_DEB_DIV_1   = 0,
    EINT_DEB_DIV_2   = 1,
    EINT_DEB_DIV_4   = 2,
    EINT_DEB_DIV_8   = 3,
    EINT_DEB_DIV_16  = 4,
    EINT_DEB_DIV_32  = 5,
    EINT_DEB_DIV_64  = 6,
    EINT_DEB_DIV_128 = 7,
} eint_debounce_div_e;

void gpio_init(uint32_t port, uint32_t pin_mask, gpio_mode_e mode, gpio_pull_e pull, gpio_drv_e drv);

void gpio_pin_init(uint32_t port, uint8_t pin_n, gpio_mode_e mode, gpio_pull_e pull, gpio_drv_e drv);

uint32_t gpio_read(uint32_t port);

uint8_t gpio_pin_get(uint32_t port, uint8_t pin_n);

void gpio_write(uint32_t port, uint32_t val);

void gpio_set(uint32_t port, uint32_t pin_mask);

void gpio_clear(uint32_t port, uint32_t pin_mask);

void gpio_pin_set(uint32_t port, uint8_t pin_n);

void gpio_pin_clear(uint32_t port, uint8_t pin_n);

void gpio_pin_toggle(uint32_t port, uint8_t pin_n);

void eint_pin_init(uint32_t int_port, uint8_t pin_n, eint_trigger_mode_e trg);

void eint_pin_enable(uint32_t int_port, uint8_t pin_n);

void eint_pin_disable(uint32_t int_port, uint8_t pin_n);

void eint_debounce_config(
    uint32_t int_port,
    eint_debounce_src_e deb_src,
    eint_debounce_div_e deb_div);

uint32_t eint_get_status(uint32_t int_port);

uint8_t eint_pin_get_status(uint32_t int_port, uint8_t pin_n);

void eint_pin_clear_status(uint32_t int_port, uint8_t pin_n);

#ifdef __cplusplus
}
#endif
