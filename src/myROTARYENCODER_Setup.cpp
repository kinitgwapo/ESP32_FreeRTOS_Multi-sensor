#include <myROTARYENCODER_Setup.hpp>

static DisplayMode currentDisplayMode = DisplayMode::TEMPERATURE;
static gpio_config_t gpio_handle;

void rotaryEncoder_GPIO_Setup(void) {
    gpio_init(&gpio_handle,(1ULL << CLK_PIN) | (1ULL << DT_PIN), GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE);
}

static void clockwise_navigate(void) {
    switch (currentDisplayMode) {
        case DisplayMode::TEMPERATURE: currentDisplayMode = DisplayMode::HUMIDITY; break;
        case DisplayMode::HUMIDITY:    currentDisplayMode = DisplayMode::LIGHT; break;
        case DisplayMode::LIGHT:       currentDisplayMode = DisplayMode::MOTION; break;
        case DisplayMode::MOTION:      currentDisplayMode = DisplayMode::TEMPERATURE; break;
    }
}

static void counterclockwise_navigate(void) {
    switch (currentDisplayMode) {
        case DisplayMode::TEMPERATURE: currentDisplayMode = DisplayMode::MOTION; break;
        case DisplayMode::MOTION:      currentDisplayMode = DisplayMode::LIGHT; break;
        case DisplayMode::LIGHT:       currentDisplayMode = DisplayMode::HUMIDITY; break;
        case DisplayMode::HUMIDITY:    currentDisplayMode = DisplayMode::TEMPERATURE; break;
    }
}

DisplayMode checkRotaryEncoder(void) {
    static bool last_clk = true;
    bool clk = gpio_get_level(CLK_PIN), dt = gpio_get_level(DT_PIN);

    if(last_clk && !clk) {
        if(dt) {
            clockwise_navigate();
        } else {
            counterclockwise_navigate();
        }
    }

    last_clk = clk;
    return currentDisplayMode;
}