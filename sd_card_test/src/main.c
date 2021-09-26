#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "io.h"
#include "display.h"
#include "arm32.h"
#include "f1c100s_gpio.h"
#include "f1c100s_clock.h"
#include "f1c100s_de.h"
#include "f1c100s_pwm.h"
#include "ff.h"

void sdcard_test(void);

int main(void)
{
    system_init(); // Initialize clocks, mmu, cache, uart, ...
    arm32_interrupt_enable(); // Enable interrupts

    display_init();
    display_set_bl(100);


    printf("PLL_CPU: %ld\n", clk_pll_get_freq(PLL_CPU));
    printf("PLL_AUDIO: %ld\n", clk_pll_get_freq(PLL_AUDIO));
    printf("PLL_VIDEO: %ld\n", clk_pll_get_freq(PLL_VIDEO));
    printf("PLL_VE: %ld\n", clk_pll_get_freq(PLL_VE));
    printf("PLL_DDR: %ld\n", clk_pll_get_freq(PLL_DDR));
    printf("PLL_PERIPH: %ld\n", clk_pll_get_freq(PLL_PERIPH));
    printf("\n");
    printf("CPU: %ld\n", clk_cpu_get_freq());
    printf("HCLK: %ld\n", clk_hclk_get_freq());
    printf("AHB: %ld\n", clk_ahb_get_freq());
    printf("APB: %ld\n", clk_apb_get_freq());
    printf("\n");

    gpio_pin_init(GPIOA, 3, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, GPIO_DRV_3);
    gpio_pin_set(GPIOA, 3);

    sdcard_test();

    while (1)
    {

    }
    return 0;
}

uint32_t img_buf[800*480*4];
void sdcard_test(void)
{
    FATFS fs;

    uint8_t state = f_mount(&fs, "", 1);
    printf("Mount: %d\r\n", state);

    debe_layer_init(0);
    debe_layer_set_addr(0, img_buf);
    debe_layer_set_mode(0, DEBE_MODE_32BPP_RGB_888);
    debe_layer_enable(0);

    FIL imgfile;

    state = f_open(&imgfile, "img.bin", FA_READ);
    printf("Open: %d\r\n", state);

    state = f_read(&imgfile, img_buf, 800*480*4, NULL);
    printf("Read: %d\r\n", state);
}

