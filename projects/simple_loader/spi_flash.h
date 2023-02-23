#ifndef SPI_FLASH_H_
#define SPI_FLASH_H_

void spi_flash_init(void);
uint32_t spi_flash_read_id(void);

void spi_flash_write_enable(void);
void spi_flash_wait_ready(void);

uint8_t spi_flash_erase_full(void);
uint8_t spi_flash_erase_sector(uint32_t addr);
uint8_t spi_flash_read(uint32_t addr, uint8_t* data, uint32_t cnt);
uint8_t spi_flash_write(uint32_t addr, uint8_t* data, uint32_t cnt);
uint8_t spi_flash_write_sector(uint16_t sector_n, uint8_t* data);

#endif /* SPI_FLASH_H_ */
