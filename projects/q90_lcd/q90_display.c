#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "display.h"
#include "f1c100s_de.h"
#include "f1c100s_gpio.h"
#include "f1c100s_pwm.h"
#include "io.h"

static void display_gpio_init(void);

void display_init(void) {
    display_gpio_init();
    gpio_pin_clear(GPIOE, 11);

    // Backlight PWM
    gpio_pin_init(GPIOE, 6, GPIO_MODE_AF3, GPIO_PULL_NONE, GPIO_DRV_3);
    pwm_init(PWM1, PWM_MODE_CONTINUOUS, 1, PWM_PSC_240); // 24M / 240 = 100kHz
    pwm_set_period(PWM1, 100); // 100k / 100 = 1kHz
    pwm_set_pulse_len(PWM1, 0);
    pwm_enable(PWM1);

    de_lcd_config_t config;

    config.width         = 320;
    config.height        = 240;
    config.bus_width     = DE_LCD_R_5BITS | DE_LCD_G_6BITS | DE_LCD_B_5BITS;
    config.bus_mode      = DE_LCD_CPU_8080;
    config.bus_8080_type = DE_8080_MODE_16BIT_3;

    config.pixel_clock_hz = 10000000;
    config.h_front_porch  = 1;
    config.h_back_porch   = 1;
    config.h_sync_len     = 1;
    config.v_front_porch  = 1;
    config.v_back_porch   = 1;
    config.v_sync_len     = 1;
    config.h_sync_invert  = 0;
    config.v_sync_invert  = 0;

    de_lcd_init(&config);

    gpio_pin_set(GPIOE, 11);

    for(volatile uint32_t i = 0; i < 100000; i++)
        ;

    de_lcd_8080_write(0x11, true);

    de_lcd_8080_write(0x3A, true);
    de_lcd_8080_write(0x05, false);

    de_lcd_8080_write(0x36, true);
    de_lcd_8080_write(0xB0, false);

    de_lcd_8080_write(0xe0, true);
    de_lcd_8080_write(0x70, false);
    de_lcd_8080_write(0x00, false);
    de_lcd_8080_write(0x06, false);
    de_lcd_8080_write(0x09, false);
    de_lcd_8080_write(0x0b, false);
    de_lcd_8080_write(0x2a, false);
    de_lcd_8080_write(0x3c, false);
    de_lcd_8080_write(0x33, false);
    de_lcd_8080_write(0x4b, false);
    de_lcd_8080_write(0x08, false);
    de_lcd_8080_write(0x16, false);
    de_lcd_8080_write(0x14, false);
    de_lcd_8080_write(0x2a, false);
    de_lcd_8080_write(0x23, false);

    de_lcd_8080_write(0xe1, true);
    de_lcd_8080_write(0xd0, false);
    de_lcd_8080_write(0x00, false);
    de_lcd_8080_write(0x06, false);
    de_lcd_8080_write(0x09, false);
    de_lcd_8080_write(0x0b, false);
    de_lcd_8080_write(0x29, false);
    de_lcd_8080_write(0x36, false);
    de_lcd_8080_write(0x54, false);
    de_lcd_8080_write(0x4b, false);
    de_lcd_8080_write(0x0d, false);
    de_lcd_8080_write(0x16, false);
    de_lcd_8080_write(0x14, false);
    de_lcd_8080_write(0x28, false);
    de_lcd_8080_write(0x22, false);

    de_lcd_8080_write(0x21, true);
    de_lcd_8080_write(0x13, true);

    de_lcd_8080_write(0x29, true);

    de_lcd_8080_write(0x2A, true);
    de_lcd_8080_write(0x00, false);
    de_lcd_8080_write(0x00, false);
    de_lcd_8080_write(0x01, false);
    de_lcd_8080_write(0x3f, false);

    de_lcd_8080_write(0x2B, true);
    de_lcd_8080_write(0x00, false);
    de_lcd_8080_write(0x00, false);
    de_lcd_8080_write(0x00, false);
    de_lcd_8080_write(0xEF, false);

    de_lcd_8080_write(0x2C, true);

    debe_set_bg_color(0x00000000);
    debe_load(DEBE_UPDATE_AUTO);

    de_lcd_8080_auto_mode(true);
}

void display_set_bl(uint8_t duty) {
    pwm_set_pulse_len(PWM1, duty);
}

static void display_gpio_init(void) {
    for(uint8_t i = 1; i <= 8; i++) {
        gpio_pin_init(GPIOD, i, GPIO_MODE_AF2, GPIO_PULL_NONE, GPIO_DRV_0);
    }
    for(uint8_t i = 10; i <= 15; i++) {
        gpio_pin_init(GPIOD, i, GPIO_MODE_AF2, GPIO_PULL_NONE, GPIO_DRV_0);
    }
    for(uint8_t i = 16; i <= 19; i++) {
        gpio_pin_init(GPIOD, i, GPIO_MODE_AF2, GPIO_PULL_NONE, GPIO_DRV_1);
    }
    gpio_pin_init(GPIOD, 20, GPIO_MODE_AF2, GPIO_PULL_UP, GPIO_DRV_1);
    gpio_pin_init(GPIOD, 21, GPIO_MODE_AF2, GPIO_PULL_UP, GPIO_DRV_1);

    gpio_pin_init(GPIOE, 11, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, GPIO_DRV_1);
    gpio_pin_set(GPIOE, 11);
}
