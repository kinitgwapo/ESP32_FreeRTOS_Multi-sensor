#include <myROTARYENCODER_Setup.hpp>

static DisplayMode currentDisplayMode = DisplayMode::TEMPERATURE;
static gpio_config_t gpio_handle;
volatile static bool encoder_changed = false;
volatile static int encoder_direction = 0; // 1 = CW, -1 =CCW
static const char *ROTARY_TAG = "Rotary Encoder";

// ISR Handler for Led & Buzzer Controller
static void IRAM_ATTR my_ISR(void *arg) {
    // Only CLK triggers this. We just check what DT is doing right now.
    int dt_level = !gpio_get_level(DT_PIN);
    
    if (dt_level == 1) {
        encoder_direction = 1;   // Clockwise
    } else {
        encoder_direction = -1;  // Counter-clockwise
    }
    encoder_changed = true;
}

void rotaryEncoder_GPIO_Setup(void) {
    // Configure CLK with an interrupt, but DT as a normal input with NO interrupt type
    gpio_init(&gpio_handle, (1ULL << CLK_PIN), GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_POSEDGE);
    gpio_init(&gpio_handle, (1ULL << DT_PIN), GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE);
    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(CLK_PIN, my_ISR, NULL);
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
    if(encoder_changed) {
        if(encoder_direction > 0) {
            clockwise_navigate();
        } else if(encoder_direction < 0) {
            counterclockwise_navigate();
        }
        encoder_changed = false;
        encoder_direction = 0;
    }

    return currentDisplayMode;
}

void rotaryEncoder_SendData(uint8_t *currentEncodeMode, uint8_t *prevcurrentEncodeMode) {
    if(*currentEncodeMode != *prevcurrentEncodeMode) {
        xQueueSend(inputQueue, currentEncodeMode, 0);
        if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
            ESP_LOGI(ROTARY_TAG, "Input Produced Mode: %d", *currentEncodeMode);
            xSemaphoreGive(serialMutex);
        }
        *prevcurrentEncodeMode = *currentEncodeMode;
    }
}