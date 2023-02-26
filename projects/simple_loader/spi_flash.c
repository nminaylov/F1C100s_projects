#include <stdint.h>
#include <string.h>
#include "io.h"
#include "spi_flash.h"
#include "f1c100s_gpio.h"
#include "f1c100s_clock.h"

enum {
    SPI_GCR = 0x04,
    SPI_TCR = 0x08,
    SPI_IER = 0x10,
    SPI_ISR = 0x14,
    SPI_FCR = 0x18,
    SPI_FSR = 0x1c,
    SPI_WCR = 0x20,
    SPI_CCR = 0x24,
    SPI_MBC = 0x30,
    SPI_MTC = 0x34,
    SPI_BCC = 0x38,
    SPI_TXD = 0x200,
    SPI_RXD = 0x300,
};

static void spi_flash_hw_init(void);
static void spi_select(void);
static void spi_deselect(void);
static void spi_write_txbuf(uint8_t* buf, uint32_t len);
static uint32_t spi_transfer(void* txbuf, void* rxbuf, uint32_t len);
static uint32_t spi_write_then_read(void* txbuf, uint32_t txlen, void* rxbuf, uint32_t rxlen);

static void spi_flash_hw_init(void) {
    uint32_t addr;
    uint32_t val;

    /* Config GPIOC0, GPIOC1, GPIOC2 and GPIOC3 */
    gpio_init(GPIOC, PIN0 | PIN1 | PIN2 | PIN3, GPIO_MODE_AF2, GPIO_PULL_NONE, GPIO_DRV_3);

    /* Deassert spi0 reset */
    clk_reset_clear(CCU_BUS_SOFT_RST0, 20);

    /* Open the spi0 bus gate */
    clk_enable(CCU_BUS_CLK_GATE0, 20);

    /* Set spi clock rate control register, divided by 4 */
    addr = SPI0_BASE;
    write32(addr + SPI_CCR, 0x00001001);

    /* Enable spi0 and do a soft reset */
    addr = SPI0_BASE;
    val  = read32(addr + SPI_GCR);
    val |= (1 << 31) | (1 << 7) | (1 << 1) | (1 << 0);
    write32(addr + SPI_GCR, val);
    while(read32(addr + SPI_GCR) & (1 << 31))
        ;

    val = read32(addr + SPI_TCR);
    val &= ~(0x3 << 0);
    val |= (1 << 6) | (1 << 2);
    write32(addr + SPI_TCR, val);

    val = read32(addr + SPI_FCR);
    val |= (1 << 31) | (1 << 15);
    write32(addr + SPI_FCR, val);
}

static void spi_select(void) {
    uint32_t addr = SPI0_BASE;
    uint32_t val;

    val = read32(addr + SPI_TCR);
    val &= ~((0x3 << 4) | (0x1 << 7));
    val |= ((0 & 0x3) << 4) | (0x0 << 7);
    write32(addr + SPI_TCR, val);
}

static void spi_deselect(void) {
    uint32_t addr = SPI0_BASE;
    uint32_t val;

    val = read32(addr + SPI_TCR);
    val &= ~((0x3 << 4) | (0x1 << 7));
    val |= ((0 & 0x3) << 4) | (0x1 << 7);
    write32(addr + SPI_TCR, val);
}

static void spi_write_txbuf(uint8_t* buf, uint32_t len) {
    uint32_t addr = SPI0_BASE;

    for(uint32_t i = 0; i < len; ++i) {
        if(buf == NULL)
            write8(addr + SPI_TXD, 0);
        else
            write8(addr + SPI_TXD, *buf++);
    }
}

static uint32_t spi_transfer(void* txbuf, void* rxbuf, uint32_t len) {
    uint32_t addr  = SPI0_BASE;
    uint32_t count = len;
    uint8_t* tx    = txbuf;
    uint8_t* rx    = rxbuf;
    uint8_t val;
    uint32_t n, i;

    // tf_rst, rf_rst
    write32(addr + SPI_FCR, read32(addr + SPI_FCR) | (1 << 31) | (1 << 15));

    while(count > 0) {
        n = (count <= 64) ? count : 64;

        write32(addr + SPI_MBC, n);
        write32(addr + SPI_MTC, n);
        write32(addr + SPI_BCC, n);

        spi_write_txbuf(tx, n);

        write32(addr + SPI_TCR, read32(addr + SPI_TCR) | (1 << 31));

        while(read32(addr + SPI_TCR) & (1 << 31))
            ;

        while((read32(addr + SPI_FSR) & 0xff) < n)
            ;
        for(i = 0; i < n; i++) {
            val = read8(addr + SPI_RXD);
            if(rx != NULL) *rx++ = val;
        }

        if(tx != NULL) tx += n;
        count -= n;
    }
    return len;
}

static uint32_t spi_write_then_read(void* txbuf, uint32_t txlen, void* rxbuf, uint32_t rxlen) {
    if(spi_transfer(txbuf, NULL, txlen) != txlen) return -1;
    if(spi_transfer(NULL, rxbuf, rxlen) != rxlen) return -1;
    return 0;
}

void sys_spi_flash_read(uint32_t addr, void* buf, uint32_t count) {
    uint8_t tx[4];

    tx[0] = 0x03;
    tx[1] = (uint8_t)(addr >> 16);
    tx[2] = (uint8_t)(addr >> 8);
    tx[3] = (uint8_t)(addr >> 0);
    spi_select();
    spi_write_then_read(tx, 4, buf, count);
    spi_deselect();
}

void spi_flash_init(void) {
    spi_flash_hw_init();

    uint8_t tx[5];

    tx[0] = 0xAB; //Reset enable
    tx[1] = 0x00;
    tx[2] = 0x00;
    tx[3] = 0x00;
    tx[4] = 0x00;

    spi_select();
    spi_transfer(tx, NULL, 5);
    spi_deselect();
    //LL_mDelay(1);

    tx[0] = 0xAB; //Reset enable
    tx[1] = 0xFF;
    tx[2] = 0xFF;

    spi_select();
    spi_transfer(tx, NULL, 3);
    spi_deselect();
}

uint32_t spi_flash_read_id(void) {
    uint8_t tx[3];
    uint32_t temp_ID = 0;

    tx[0] = 0x9F; //JEDEC ID

    spi_select();
    spi_transfer(tx, NULL, 1);
    spi_transfer(NULL, tx, 3);
    spi_deselect();

    temp_ID = tx[0] << 16;
    temp_ID |= tx[1] << 8;
    temp_ID |= tx[2];

    return temp_ID;
}

uint8_t spi_flash_erase_full(void) {
    uint8_t flash_state = 0;
    uint8_t cmd         = 0x05;

    spi_select();
    spi_transfer(&cmd, NULL, 1);
    spi_transfer(NULL, &flash_state, 1);
    while(flash_state & 0x01) {
        spi_transfer(NULL, &flash_state, 1);
    }
    spi_deselect();
    //LL_mDelay(1);

    spi_select();
    cmd = 0x06;
    spi_transfer(&cmd, NULL, 1);
    spi_deselect();
    //LL_mDelay(1);

    spi_select();
    cmd = 0xC7;
    spi_transfer(&cmd, NULL, 1);
    spi_deselect();
    //LL_mDelay(1);
    return 1;
}

uint8_t spi_flash_erase_sector(uint32_t addr) {
    uint8_t tx[4];

    tx[0] = 0x20;
    tx[1] = (uint8_t)(addr >> 16);
    tx[2] = (uint8_t)(addr >> 8);
    tx[3] = (uint8_t)(addr >> 0);

    spi_flash_wait_ready();

    spi_flash_write_enable();

    spi_select();
    spi_transfer(tx, NULL, 4);
    spi_deselect();
    return 1;
}

void spi_flash_write_enable(void) {
    spi_select();
    uint8_t cmd = 0x06;
    spi_transfer(&cmd, NULL, 1);
    spi_deselect();
}

void spi_flash_wait_ready(void) {
    uint8_t flash_state = 0;
    uint8_t cmd         = 0x05;

    spi_select();
    spi_transfer(&cmd, NULL, 1);
    spi_transfer(NULL, &flash_state, 1);
    while(flash_state & 0x01) {
        spi_transfer(NULL, &flash_state, 1);
    }
    spi_deselect();
}

uint8_t spi_flash_read(uint32_t addr, uint8_t* data, uint32_t cnt) {
    uint8_t tx[4];

    tx[0] = 0x03;
    tx[1] = (uint8_t)(addr >> 16);
    tx[2] = (uint8_t)(addr >> 8);
    tx[3] = (uint8_t)(addr >> 0);

    spi_select();
    spi_transfer(tx, NULL, 4);
    spi_transfer(NULL, data, cnt);
    spi_deselect();
    return 1;
}

uint8_t spi_flash_write(uint32_t addr, uint8_t* data, uint32_t cnt) {
    uint8_t tx[4];

    tx[0] = 0x02;
    tx[1] = (uint8_t)(addr >> 16);
    tx[2] = (uint8_t)(addr >> 8);
    tx[3] = (uint8_t)(addr >> 0);

    spi_flash_wait_ready();

    spi_flash_write_enable();

    spi_select();
    spi_transfer(tx, NULL, 4);
    spi_transfer(data, NULL, cnt);
    spi_deselect();
    return 1;
}

uint8_t spi_flash_write_sector(uint16_t sector_n, uint8_t* data) {
    uint32_t start_addr = sector_n * 4096;

    spi_flash_erase_sector(start_addr);

    for(uint8_t i = 0; i < 16; i++) {
        spi_flash_write(start_addr + i * 256, data + i * 256, 256);
    }

    spi_flash_wait_ready();

    return 1;
}
