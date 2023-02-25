#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "f1c100s_periph.h"

#define UART0 UART0_BASE
#define UART1 UART1_BASE
#define UART2 UART2_BASE

typedef enum {
    UART_RBR = 0x00,
    UART_THR = 0x00,
    UART_DLL = 0x00,
    UART_IER = 0x04,
    UART_DLH = 0x04,
    UART_IIR = 0x08,
    UART_FCR = 0x08,
    UART_LCR = 0x0C,
    UART_MCR = 0x10,
    UART_LSR = 0x14,
    UART_MSR = 0x18,
    UART_SCR = 0x1C,

    UART_USR     = 0x7C,
    UART_TFL     = 0x80,
    UART_RFL     = 0x84,
    UART_HSK     = 0x88,
    UART_HALT    = 0xA4,
    UART_DBG_DLL = 0xB0,
    UART_DBG_DLH = 0xB4,
} uart_reg_e;

typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD  = 1,
    UART_PARITY_EVEN = 3,
} uart_parity_e;

typedef enum {
    UART_LEN_5B = 0,
    UART_LEN_6B = 1,
    UART_LEN_7B = 2,
    UART_LEN_8B = 3,
} uart_len_e;

typedef enum {
    UART_IEN_RBF   = 0,
    UART_IEN_TBE   = 1,
    UART_IEN_LINE  = 2,
    UART_IEN_MODEM = 3,
    UART_IEN_THRE  = 7,
} uart_int_en_e;

typedef enum {
    UART_IID_MODEM   = 0x00,
    UART_IID_NONE    = 0x01,
    UART_IID_TX_E    = 0x02,
    UART_IID_RX_NE   = 0x04,
    UART_IID_RX_ER   = 0x06,
    UART_IID_BUSY    = 0x07,
    UART_IID_TIMEOUT = 0x0C,
} uart_int_id_e;

typedef enum {
    UART_LSR_DR      = (1 << 0),
    UART_LSR_OE      = (1 << 1),
    UART_LSR_PE      = (1 << 2),
    UART_LSR_FE      = (1 << 3),
    UART_LSR_BI      = (1 << 4),
    UART_LSR_THRE    = (1 << 5),
    UART_LSR_TEMT    = (1 << 6),
    UART_LSR_FIFOERR = (1 << 7),
} uart_line_status_e;

void uart_init(uint32_t uart, uint32_t baud);

void uart_set_baudrate(uint32_t uart, uint32_t baud);

void uart_set_parity(uint32_t uart, uart_parity_e par);

void uart_set_data_bits(uint32_t uart, uart_len_e len);

void uart_tx(uint32_t uart, uint8_t data);

uint8_t uart_get_rx(uint32_t uart);

uart_int_id_e uart_get_int_id(uint32_t uart);

uint8_t uart_get_status(uint32_t uart);

#ifdef __cplusplus
}
#endif
