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


















/*
void pir_InActiveTOActive(bool *status, bool *prevstatus) {
    if(*status == true && *prevstatus == false) {
        if(!(xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE)) {
            if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                ESP_LOGI(PIR_TAG, "Motion Detected! State changed: INACTIVE -> ACTIVE");
                xSemaphoreGive(serialMutex);
            }
            xEventGroupSetBits(systemEventGroup, EVENT_ACTIVE);
        } else {
            if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                ESP_LOGI(PIR_TAG, "Motion Detected!");
                xSemaphoreGive(serialMutex);
            }
        }
    }

}

void pir_Active(bool *status, TickType_t *lastMotionTick) {
    if(*status == true) {
        xEventGroupSetBits(systemEventGroup, EVENT_MOTION);
        *lastMotionTick = xTaskGetTickCount();
    } else {
        xEventGroupClearBits(systemEventGroup, EVENT_MOTION);
    }
}

void pir_ActiveTOInActive(TickType_t *lastMotionTick) {
    if(xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE) {
        if((xTaskGetTickCount() - *lastMotionTick) > pdMS_TO_TICKS(15000)) {
            xEventGroupClearBits(systemEventGroup, EVENT_ACTIVE);
            if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                ESP_LOGI(PIR_TAG, "Inactivity timeout (15s) reached! State changed: ACTIVE -> INACTIVE");
                xSemaphoreGive(serialMutex);
            }
        }
    }
}
*/