#include <myBUZZER_Setup.hpp>

static gpio_config_t BUZZER_Pin;
static AlarmState currentTemperatureMode;

void buzzerPin_Setup(uint64_t pin_mask) {
    gpio_init(&BUZZER_Pin, pin_mask, GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE);
}

AlarmState evaluateTemperature(float temperature) {
    if(temperature < 10.0f) {
        return currentTemperatureMode = AlarmState::LOW_TEMPERATURE;
    } else if(temperature > 35.0f) {
        return currentTemperatureMode = AlarmState::HIGH_TEMPERATURE;
    }
    return currentTemperatureMode = AlarmState::NORMAL;
}