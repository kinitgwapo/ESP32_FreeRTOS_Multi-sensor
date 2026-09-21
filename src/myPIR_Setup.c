#include <myPIR_Setup.h>

static gpio_config_t PIR_Pin;
static const char *PIR_TAG = "PIR";

void pirPin_Setup(uint64_t pin_mask) {
    gpio_init(&PIR_Pin, pin_mask, GPIO_MODE_INPUT, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE);
}

void pir_evaluateMotion(bool status, bool *prevstatus) {
    if(status) {
        xEventGroupSetBits(systemEventGroup, EVENT_MOTION);
        if(*prevstatus == false) {
            if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                ESP_LOGI(PIR_TAG, "Motion Sensor Triggered!");
                xSemaphoreGive(serialMutex);
            }
        }
    } else {
        xEventGroupClearBits(systemEventGroup, EVENT_MOTION);
    }

    *prevstatus = status;
}