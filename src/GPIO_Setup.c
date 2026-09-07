#include <GPIO_Setup.h>

void gpio_init(gpio_config_t *config, uint64_t pin_mask, gpio_mode_t gpio_mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t intr_type) {
    *config = (gpio_config_t) {
        .pin_bit_mask = pin_mask,
        .mode = gpio_mode,
        .pull_up_en = pullup,
        .pull_down_en = pulldown,
        .intr_type = intr_type
    };

    // Save GPIO Configuration
    gpio_config(config);
}