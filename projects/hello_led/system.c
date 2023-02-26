#include "system.h"
#include <stdint.h>
#include "armv5_mmu.h"
#include "arm32.h"
#include "sizes.h"
#include "f1c100s_clock.h"
#include "f1c100s_intc.h"
#include "f1c100s_gpio.h"
#include "f1c100s_uart.h"
#include "io.h"

static void sys_clk_init(void);
static void sys_uart_init(void);
static void sys_mmu_cache_init(void);

static inline void sdelay(int loops);

// MMU translation table
uint32_t mmu_l1_tbl[4096] __attribute__((section(".mmu_tbl")));

void system_init(void) {
    sys_clk_init();
    sys_uart_init();
    sys_mmu_cache_init();
    intc_init();
}

static void sys_clk_init(void) {
    // Set CPU clock to 24MHz
    clk_cpu_config(CLK_CPU_SRC_OSC24M);
    sdelay(10); // Wait for some clock cycles

    clk_pll_init(PLL_PERIPH, 25, 1); // PLL_PERIPH = 24M*25/1 = 600M
    clk_pll_enable(PLL_PERIPH);
    while(!clk_pll_is_locked(PLL_PERIPH))
        ; // Wait for PLL to lock

    // Configure bus clocks
    clk_hclk_config(1); // HCLK = CLK_CPU
    clk_ahb_config(CLK_AHB_SRC_PLL_PERIPH_PREDIV, 3, 1); // AHB = PLL_PERIPH/3/1 = 200M
    clk_apb_config(CLK_APB_DIV_2); // APB = AHB/2 = 100M
    sdelay(10);

    // Configure video clocks
    clk_pll_init(PLL_VIDEO, 99, 8); // 24*99/8 = 297MHz
    clk_pll_enable(PLL_VIDEO);

    // Configure CPU PLL
    clk_pll_init(PLL_CPU, 30, 1); // PLL_CPU = 24M*30/1 = 720M
    clk_pll_enable(PLL_CPU);
    while(!clk_pll_is_locked(PLL_CPU))
        ; // Wait for PLL to lock

    // Select PLL as CPU clock source
    clk_cpu_config(CLK_CPU_SRC_PLL_CPU);
    sdelay(10);
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

static void sys_mmu_cache_init(void) {
    // Initialize translation table
    mmu_map_l1_entry(mmu_l1_tbl, 0x00000000, 0x00000000, SZ_2G, SECTION_NCNB);
    mmu_map_l1_entry(mmu_l1_tbl, 0x80000000, 0x80000000, SZ_2G, SECTION_NCNB);
    // Enable caching for first 16MB of DRAM
    mmu_map_l1_entry(mmu_l1_tbl, 0x80000000, 0x80000000, SZ_1M * 16, SECTION_CB);

    arm32_ttb_set((uint32_t)(mmu_l1_tbl));
    arm32_tlb_invalidate();
    arm32_domain_set(0x3); // Domain access - manager
    arm32_mmu_enable();
    arm32_icache_enable();
    arm32_dcache_enable();
}

static inline void sdelay(int loops) {
    __asm__ __volatile__("1:\n"
                         "subs %0, %1, #1\n"
                         "bne 1b"
                         : "=r"(loops)
                         : "0"(loops));
}
