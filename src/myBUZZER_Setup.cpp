#include <myBUZZER_Setup.hpp>

static gpio_config_t BUZZER_Pin;
static AlarmState currentTemperatureMode;
static uint8_t pin_num;

void buzzerPin_Setup(uint64_t pin_mask) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    gpio_init(&BUZZER_Pin, pin_mask, GPIO_MODE_OUTPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE);
    pin_num = __builtin_ffsll(pin_mask) - 1; // Convert bit_mask to GPIO_NUM
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 2000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .gpio_num = pin_num,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 127,
        .hpoint = 0,
    };

    ledc_channel_config(&channel_config);

    #pragma GCC diagnostic pop
}

AlarmState evaluateTemperature(float temperature) {
    if(temperature <= 18.0f) {
        return currentTemperatureMode = AlarmState::LOW_TEMPERATURE;
    } else if(temperature >= 30.0f) {
        return currentTemperatureMode = AlarmState::HIGH_TEMPERATURE;
    }
    return currentTemperatureMode = AlarmState::NORMAL;
}

void buzzer_Update(AlarmState currentResult) {
    switch(currentResult) {
        case AlarmState::NORMAL:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            break;
        case AlarmState::LOW_TEMPERATURE:
        case AlarmState::HIGH_TEMPERATURE:
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 127);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            break;
    }
}