#include <stdint.h>
#include "q90_buttons.h"
#include "f1c100s_gpio.h"

void q90_buttons_init(void) {
    gpio_init(GPIOA, PIN1, GPIO_MODE_INPUT, GPIO_PULL_UP, GPIO_DRV_0);
    gpio_init(GPIOC, PIN1 | PIN2, GPIO_MODE_INPUT, GPIO_PULL_UP, GPIO_DRV_0);
    gpio_init(GPIOD, PIN0 | PIN9, GPIO_MODE_INPUT, GPIO_PULL_UP, GPIO_DRV_0);
    gpio_init(GPIOE, PIN0 | PIN1 | PIN2 | PIN3 | PIN4, GPIO_MODE_INPUT, GPIO_PULL_UP, GPIO_DRV_0);
    gpio_init(GPIOE, PIN5 | PIN7 | PIN8 | PIN9, GPIO_MODE_INPUT, GPIO_PULL_UP, GPIO_DRV_0);
}

uint32_t q90_buttons_read(void) {
    uint32_t bnt_val = 0;

    if(gpio_pin_get(GPIOA, 1) == 0) bnt_val |= BTN_PLUS;
    if(gpio_pin_get(GPIOC, 1) == 0) bnt_val |= BTN_L;
    if(gpio_pin_get(GPIOC, 2) == 0) bnt_val |= BTN_R;
    if(gpio_pin_get(GPIOD, 0) == 0) bnt_val |= BTN_SELECT;
    if(gpio_pin_get(GPIOD, 9) == 0) bnt_val |= BTN_X;
    if(gpio_pin_get(GPIOE, 0) == 0) bnt_val |= BTN_START;
    if(gpio_pin_get(GPIOE, 1) == 0) bnt_val |= BTN_MINUS;
    if(gpio_pin_get(GPIOE, 2) == 0) bnt_val |= BTN_UP;
    if(gpio_pin_get(GPIOE, 3) == 0) bnt_val |= BTN_DONW;
    if(gpio_pin_get(GPIOE, 4) == 0) bnt_val |= BTN_LEFT;
    if(gpio_pin_get(GPIOE, 5) == 0) bnt_val |= BTN_RIGHT;
    if(gpio_pin_get(GPIOE, 7) == 0) bnt_val |= BTN_B;
    if(gpio_pin_get(GPIOE, 8) == 0) bnt_val |= BTN_Y;
    if(gpio_pin_get(GPIOE, 9) == 0) bnt_val |= BTN_A;

    return bnt_val;
}
