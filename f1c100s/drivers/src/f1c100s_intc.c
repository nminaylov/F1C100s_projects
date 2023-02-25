#include "f1c100s_intc.h"
#include "io.h"
#include <stddef.h>
#include <string.h>

static intc_irq_handler irq_handlers[41];

void intc_enable_irq(intc_irq_vector_e irq) {
    if(irq < 32)
        set32(INTC_BASE + INTC_ENABLE0, (1 << irq));
    else
        set32(INTC_BASE + INTC_ENABLE1, (1 << (irq - 32)));
}

void intc_disable_irq(intc_irq_vector_e irq) {
    if(irq < 32)
        clear32(INTC_BASE + INTC_ENABLE0, (1 << irq));
    else
        clear32(INTC_BASE + INTC_ENABLE1, (1 << (irq - 32)));
}

void intc_set_priority(intc_irq_vector_e irq, uint8_t prio) {
    uint32_t reg = INTC_BASE + INTC_PRIORITY0 + irq / 16 * 4;

    uint32_t val = read32(reg) & ~(0x3 << ((irq % 16) * 2));
    write32(reg, val | (prio << ((irq % 16) * 2)));
}

void intc_set_irq_handler(intc_irq_vector_e irq, intc_irq_handler handler) {
    irq_handlers[irq] = handler;
}

void intc_init(void) {
    write32(INTC_BASE + INTC_ENABLE0, 0); // Disable all interrupts
    write32(INTC_BASE + INTC_ENABLE1, 0);
    memset(irq_handlers, 0, sizeof(irq_handlers)); // Clear handlers table
    write32(INTC_BASE + INTC_BASE_ADDR, 0); // Set offset to 0
}

// Global IRQ handler
void irq_handler(void) {
    uint32_t irq_src = read32(INTC_BASE) >> 2;

    if(irq_handlers[irq_src] != NULL)
        irq_handlers[irq_src]();
    else
        intc_disable_irq(irq_src); // Disable undefined IRQ, not to get stuck in it
}
