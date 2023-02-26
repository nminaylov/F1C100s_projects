#include "system.h"
#include <stdint.h>
#include "arm32.h"
#include "sizes.h"
#include "f1c100s_clock.h"
#include "f1c100s_intc.h"
#include "f1c100s_gpio.h"
#include "f1c100s_uart.h"
#include "io.h"

static void sys_clk_init(void);
static void sys_uart_init(void);

static inline void sdelay(int loops);

void system_init(void) {
    sys_clk_init();
    sys_uart_init();
}

static void sys_clk_init(void) {
    // Set CPU clock to 24MHz
    clk_cpu_config(CLK_CPU_SRC_OSC24M);
    sdelay(100); // Wait for some clock cycles

    // Configure CPU PLL
    clk_pll_disable(PLL_CPU);
    clk_pll_init(PLL_CPU, 30, 1); // PLL_CPU = 24M*30/1 = 720M
    clk_pll_enable(PLL_CPU);
    while(!clk_pll_is_locked(PLL_CPU))
        ; // Wait for PLL to lock

    // Configure peripheral clock
    clk_pll_disable(PLL_PERIPH);
    clk_pll_init(PLL_PERIPH, 25, 1); // PLL_PERIPH = 24M*25/1 = 600M
    clk_pll_enable(PLL_PERIPH);
    while(!clk_pll_is_locked(PLL_PERIPH))
        ; // Wait for PLL to lock

    // Configure bus clocks
    //write32(CCU_BASE + CCU_AHB_APB_CFG, 0x00003180); // TODO:
    clk_hclk_config(1); // HCLK = CLK_CPU
    clk_apb_config(CLK_APB_DIV_2); // APB = AHB/2 = 100M
    clk_ahb_config(CLK_AHB_SRC_PLL_PERIPH_PREDIV, 3, 1); // AHB = PLL_PERIPH/3/1 = 200M
    sdelay(100);

    // Select PLL as CPU clock source
    clk_cpu_config(CLK_CPU_SRC_PLL_CPU);
    sdelay(100);
}

static void sys_uart_init(void) {
    gpio_init(
        GPIOE, PIN0 | PIN1, GPIO_MODE_AF5, GPIO_PULL_NONE, GPIO_DRV_3); // Configure GPIO pins
    clk_enable(CCU_BUS_CLK_GATE2, 20); // Open the clock gate for uart0
    clk_reset_clear(CCU_BUS_SOFT_RST2, 20); // Deassert uart0 reset
    uart_init(UART0, 115200); // Configure UART0 to 115200-8-n-1
}

void putchar_(char c) {
    while(!(uart_get_status(UART0) & UART_LSR_THRE))
        ;
    uart_tx(UART0, c);
}

static inline void sdelay(int loops) {
    __asm__ __volatile__("1:\n"
                         "subs %0, %1, #1\n"
                         "bne 1b"
                         : "=r"(loops)
                         : "0"(loops));
}
