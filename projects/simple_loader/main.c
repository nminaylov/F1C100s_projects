#include <stdint.h>
#include <stdio.h>
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
    printf("simple_loader started\r\n");

    gpio_init(GPIOF, PIN0 | PIN1 | PIN3 | PIN5, GPIO_MODE_AF3, GPIO_PULL_NONE, GPIO_DRV_3);
    printf("JTAG enabled\r\n");

    uint8_t size = sys_dram_init();
    printf("DRAM size: %uMB\r\n", size);

    spi_flash_init();
    uint32_t id = spi_flash_read_id();
    printf("SPI flash ID: %06lX\r\n", id);

    spi_flash_read(FW_START_ADDR, dram, sizeof(fw_header_t));
    fw_header_t* hdr = (fw_header_t*)dram;
    if((hdr->magic == FW_MAGIC) && (hdr->end > hdr->start)) {
        uint32_t len = hdr->end - hdr->start;
        printf("FW header found! len: %lu\r\n", len);
        spi_flash_read(FW_START_ADDR, dram, len);
        printf("Read done, jump to FW\r\n");
        __asm__ __volatile__("ldr pc, =0x80000000" : : : "memory");
    } else {
        printf("No valid header found\r\n");
    }

    while(1) {
    }

    return 0;
}

void dump_regs(uint32_t addr, uint32_t len) {
    for(uint32_t i = addr; i < addr + len; i += 4) {
        printf("%08lX: %08lX\r\n", i, read32(i));
    }
}

void reset_cpu(void) {
    arm32_interrupt_disable();
    wdg_init(WDG_MODE_RESET, WDG_INTV_500MS);
    while(1)
        ;
}
