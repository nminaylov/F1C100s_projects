#include "f1c100s_touch.h"
#include "io.h"

void tp_init(void) {
    uint32_t val = (0xF << 24) | (1 << 23); // ADC_FIRST_DLY ?
    val |= (0 << 22) | (0 << 20); // CLK_IN: 24M/2
    val |= (7 << 16); // FS: CLK_IN/2^(20-7) = CLK_IN/8192 =~ 1464Hz
    val |= (63 << 0); // TACQ ?
    write32(RTP_BASE + TP_CTRL0, val);

    val = (5 << 12) | (1 << 9); // Debounce
    val |= (0 << 6); // Single point
    val |= (1 << 5); // TP_EN
    write32(RTP_BASE + TP_CTRL1, val);

    val = (8 << 28); // Sensitivity
    val |= (8 << 26); // FIFO: x,y
    val |= (0 << 24); // No pressure measurement
    val |= (0xFFF << 0); // PRE_MEA_THRE_CNT - default
    write32(RTP_BASE + TP_CTRL2, val);

    val = (1 << 2) | (2 << 0); // Filter: 8/4?
    write32(RTP_BASE + TP_CTRL3, val);

    tp_fifo_flush();
    tp_fifo_set_trig_level(2);
}

void tp_int_config(uint32_t int_mask) {
    uint32_t val = read32(RTP_BASE + TP_INT_FIFO_CTRL);
    val &= ~(TP_INT_OVERRUN | TP_INT_FIFO_DATA | TP_INT_UP | TP_INT_DOWN);
    write32(RTP_BASE + TP_INT_FIFO_CTRL, val | int_mask);
}

uint32_t tp_int_get_state(void) {
    uint32_t val = read32(RTP_BASE + TP_INT_FIFO_CTRL);
    val &= (TP_INT_OVERRUN | TP_INT_FIFO_DATA | TP_INT_UP | TP_INT_DOWN);
    return val;
}

void tp_int_clear(uint32_t int_mask) {
    set32(RTP_BASE + TP_INT_FIFO_STAT, int_mask);
}

void tp_fifo_flush(void) {
    set32(RTP_BASE + TP_INT_FIFO_STAT, (1 << 4));
}

void tp_fifo_set_trig_level(uint8_t lvl) {
    uint32_t val = read32(RTP_BASE + TP_INT_FIFO_CTRL) & ~(0x1F << 8);
    write32(RTP_BASE + TP_INT_FIFO_CTRL, val | lvl);
}

void tp_fifo_read(uint16_t* data, uint8_t len) {
    for(uint8_t i = 0; i < len; i++) {
        data[i] = read32(RTP_BASE + TP_DATA);
    }
}
