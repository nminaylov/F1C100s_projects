#include <stdint.h>
#include "printf.h"
#include "system.h"
#include <string.h>
#include "io.h"
#include "arm32.h"
#include "sys_dram.h"
#include "spi_flash.h"
#include "f1c100s_timer.h"
#include "f1c100s_gpio.h"

void dump_regs(uint32_t addr, uint32_t len);
void reset_cpu(void);

#define FW_START_ADDR 0x10000
#define FW_MAGIC 0x555AAAFF
uint8_t* dram = (uint8_t*)0x80000000;

typedef struct {
    uint32_t jump_instr;
    uint32_t magic;
    uint32_t start;
    uint32_t end;
    uint32_t reserved;
    uint32_t crc32;
} fw_header_t;

int main(void) {
    system_init();
    printf_("simple_loader started\r\n");

    gpio_init(GPIOF, PIN0 | PIN1 | PIN3 | PIN5, GPIO_MODE_AF3, GPIO_PULL_NONE, GPIO_DRV_3);
    printf_("JTAG enabled\r\n");

    uint8_t size = sys_dram_init();
    printf_("DRAM size: %uMB\r\n", size);

    spi_flash_init();
    uint32_t id = spi_flash_read_id();
    printf_("SPI flash ID: %06X\r\n", id);

    spi_flash_read(FW_START_ADDR, dram, sizeof(fw_header_t));
    fw_header_t* hdr = (fw_header_t*)dram;
    if((hdr->magic == FW_MAGIC) && (hdr->end > hdr->start)) {
        uint32_t len = hdr->end - hdr->start;
        printf_("FW header found! len: %u\r\n", len);
        spi_flash_read(FW_START_ADDR, dram, len);
        printf_("Read done, jump to FW\r\n");
        __asm__ __volatile__("ldr pc, =0x80000000" : : : "memory");
    } else {
        printf_("No valid header found\r\n");
    }

    while(1) {
    }

    return 0;
}

void dump_regs(uint32_t addr, uint32_t len) {
    for(uint32_t i = addr; i < addr + len; i += 4) {
        printf_("%08X: %08X\r\n", i, read32(i));
    }
}

void reset_cpu(void) {
    arm32_interrupt_disable();
    wdg_init(WDG_MODE_RESET, WDG_INTV_500MS);
    while(1)
        ;
}
