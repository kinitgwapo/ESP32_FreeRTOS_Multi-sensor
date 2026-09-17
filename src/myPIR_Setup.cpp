#include <myPIR_Setup.hpp>

SystemState currentSystemState = SystemState::ACTIVE;
static gpio_config_t PIR_Pin;

void pirPin_Setup(uint64_t pin_mask) {
    gpio_init(&PIR_Pin, pin_mask, GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE);
}