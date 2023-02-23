#include "f1c100s_gpio.h"
#include "io.h"

// Configure multiple gpio pins
void gpio_init(
    uint32_t port,
    uint32_t pin_mask,
    gpio_mode_e mode,
    gpio_pull_e pull,
    gpio_drv_e drv) {
    for(uint8_t i = 0; i < 32; i++) {
        if(pin_mask & (1U << i)) gpio_pin_init(port, i, mode, pull, drv);
    }
}

// Configure single GPIO pin
void gpio_pin_init(
    uint32_t port,
    uint8_t pin_n,
    gpio_mode_e mode,
    gpio_pull_e pull,
    gpio_drv_e drv) {
    uint32_t reg = 0;
    uint32_t val = 0;

    // Set pin mode
    reg = port + GPIO_CFG0 + (pin_n / 8) * 4; // Get CFG register address
    val = read32(reg);
    val &= ~(0x7 << ((pin_n % 8) * 4)); // Clear mode bits
    val |= ((mode & 0x7) << ((pin_n % 8) * 4)); // Set new mode
    write32(reg, val);

    // Set drive strength
    reg = port + GPIO_DRV0 + (pin_n / 16) * 4;
    val = read32(reg);
    val &= ~(0x3 << ((pin_n % 16) * 2));
    val |= ((drv & 0x3) << ((pin_n % 16) * 2));
    write32(reg, val);

    // Set pull configuration
    reg = port + GPIO_PUL0 + (pin_n / 16) * 4;
    val = read32(reg);
    val &= ~(0x3 << ((pin_n % 16) * 2));
    val |= ((pull & 0x3) << ((pin_n % 16) * 2));
    write32(reg, val);
}

inline uint32_t gpio_read(uint32_t port) {
    return (read32(port + GPIO_DATA));
}

inline uint8_t gpio_pin_get(uint32_t port, uint8_t pin_n) {
    return ((read32(port + GPIO_DATA) >> pin_n) & 0x1);
}

inline void gpio_write(uint32_t port, uint32_t val) {
    write32(port + GPIO_DATA, val);
}

inline void gpio_set(uint32_t port, uint32_t pin_mask) {
    write32(port + GPIO_DATA, (read32(port + GPIO_DATA) | (pin_mask)));
}

inline void gpio_clear(uint32_t port, uint32_t pin_mask) {
    write32(port + GPIO_DATA, (read32(port + GPIO_DATA) & (~pin_mask)));
}

inline void gpio_pin_set(uint32_t port, uint8_t pin_n) {
    write32(port + GPIO_DATA, (read32(port + GPIO_DATA) | (1 << pin_n)));
}

inline void gpio_pin_clear(uint32_t port, uint8_t pin_n) {
    write32(port + GPIO_DATA, (read32(port + GPIO_DATA) & (~(1 << pin_n))));
}

inline void gpio_pin_toggle(uint32_t port, uint8_t pin_n) {
    write32(port + GPIO_DATA, (read32(port + GPIO_DATA) ^ (1 << pin_n)));
}

void eint_pin_init(uint32_t int_port, uint8_t pin_n, eint_trigger_mode_e trg) {
    uint32_t reg = int_port + GPIO_INT_CFG0 + (pin_n / 6) * 4;
    uint32_t val = read32(reg);
    val &= ~(0xF << ((pin_n % 6) * 4));
    val |= ((trg & 0x7) << ((pin_n % 6) * 4));
    write32(reg, val);
}

inline void eint_pin_enable(uint32_t int_port, uint8_t pin_n) {
    write32(int_port + GPIO_INT_CTRL, (read32(int_port + GPIO_INT_CTRL) | (1 << pin_n)));
}

inline void eint_pin_disable(uint32_t int_port, uint8_t pin_n) {
    write32(int_port + GPIO_INT_CTRL, (read32(int_port + GPIO_INT_CTRL) & (~(1 << pin_n))));
}

void eint_debounce_config(
    uint32_t int_port,
    eint_debounce_src_e deb_src,
    eint_debounce_div_e deb_div) {
    write32(int_port + GPIO_INT_DEB, deb_src | (deb_div << 4));
}

inline uint32_t eint_get_status(uint32_t int_port) {
    return (read32(int_port + GPIO_INT_STA));
}

inline uint8_t eint_pin_get_status(uint32_t int_port, uint8_t pin_n) {
    return (read32(int_port + GPIO_INT_STA) >> pin_n) & 0x1;
}

inline void eint_pin_clear_status(uint32_t int_port, uint8_t pin_n) {
    set32(int_port + GPIO_INT_STA, (0x1 << pin_n));
}
