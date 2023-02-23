#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "display.h"
#include "f1c100s_de.h"
#include "f1c100s_gpio.h"
#include "f1c100s_pwm.h"

static void display_gpio_init(void);

void display_init(void)
{
    de_lcd_config_t config;

    config.width = 800;
    config.height = 480;
    config.bus_width = DE_LCD_R_6BITS | DE_LCD_G_6BITS | DE_LCD_B_6BITS;

    config.pixel_clock_hz = 33000000;
    config.h_front_porch = 40;
    config.h_back_porch = 87;
    config.h_sync_len = 1;
    config.v_front_porch = 13;
    config.v_back_porch = 31;
    config.v_sync_len = 3;
    config.h_sync_invert = 1;
    config.v_sync_invert = 1;

    display_gpio_init();
    de_lcd_init(&config);
    
    gpio_pin_init(GPIOE, 6, GPIO_MODE_AF3, GPIO_PULL_NONE, GPIO_DRV_3);
    pwm_init(PWM1, PWM_MODE_CONTINUOUS, 1, PWM_PSC_240); // 24M / 240 = 100kHz
    pwm_set_period(PWM1, 100); // 100k / 100 = 1kHz
    pwm_set_pulse_len(PWM1, 0);
    pwm_enable(PWM1);

    debe_set_bg_color(0x00000000);
    debe_load(DEBE_UPDATE_AUTO);
}

void display_set_bl(uint8_t duty)
{
    pwm_set_pulse_len(PWM1, duty);
}

static void display_gpio_init(void)
{
    for (uint8_t i = 0; i <= 12; i++)
    {
        gpio_pin_init(GPIOD, i, GPIO_MODE_AF2, GPIO_PULL_NONE, GPIO_DRV_3);
    }
    for (uint8_t i = 13; i <= 21; i++)
    {
        gpio_pin_init(GPIOD, i, GPIO_MODE_AF2, GPIO_PULL_NONE, GPIO_DRV_3);
    }
}

