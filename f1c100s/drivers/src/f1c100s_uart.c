#include "f1c100s_uart.h"
#include "f1c100s_clock.h"
#include "io.h"

// Initialise UART with default settings (no parity, 8bits, 1 stop bit, no flow control)
void uart_init(uint32_t uart, uint32_t baud) {
    write32(uart + UART_IER, 0x00); // Disable interrupts
    write32(uart + UART_FCR, 0x07); // Enable and reset FIFO
    write32(uart + UART_MCR, 0x00); // Disable flow control

    uart_set_baudrate(uart, baud);

    uint32_t val = read32(uart + UART_LCR) & ~0x3F;
    val |= (UART_LEN_8B << 0) | (0 << 2) | (UART_PARITY_NONE << 3); // 8bit, parity off, 1 stop
    write32(uart + UART_LCR, val);
}

void uart_set_baudrate(uint32_t uart, uint32_t baud) {
    uint32_t apb_clock = clk_apb_get_freq();

    uint16_t val = (uint16_t)(apb_clock / baud / 16UL);

    write32(uart + UART_LCR, (read32(uart + UART_LCR) | (1 << 7))); // Divisor Latch Access bit set
    write32(uart + UART_DLL, val & 0xFF); // Write divisor value
    write32(uart + UART_DLH, (val >> 8) & 0xFF);
    write32(
        uart + UART_LCR, (read32(uart + UART_LCR) & ~(1 << 7))); // Divisor Latch Access bit clear
}

void uart_set_parity(uint32_t uart, uart_parity_e par) {
    uint32_t val = read32(uart + UART_LCR) & ~0x38;
    write32(uart + UART_LCR, val | (par << 3));
}

void uart_set_data_bits(uint32_t uart, uart_len_e len) {
    uint32_t val = read32(uart + UART_LCR) & ~0x03;
    write32(uart + UART_LCR, val | len);
}

inline void uart_tx(uint32_t uart, uint8_t data) {
    write32(uart + UART_THR, data);
}

inline uint8_t uart_get_rx(uint32_t uart) {
    return (uint8_t)read32(uart + UART_RBR);
}

inline void uart_enable_interrupt(uint32_t uart, uart_int_en_e int_n) {
    write32(uart + UART_IER, (read32(uart + UART_IER) | (1 << int_n)));
}

inline void uart_disable_interrupt(uint32_t uart, uart_int_en_e int_n) {
    write32(uart + UART_IER, (read32(uart + UART_IER) & ~(1 << int_n)));
}

inline uart_int_id_e uart_get_int_id(uint32_t uart) {
    return (read32(uart + UART_IIR) & 0x0F);
}

inline uint8_t uart_get_status(uint32_t uart) {
    return (uint8_t)read32(uart + UART_LSR);
}
