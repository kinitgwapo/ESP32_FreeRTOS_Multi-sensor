#include "myRTOS_Logic.h"
#include <esp_log.h>

static const char *RTOS_TAG = "RTOS_SETUP";

// --- Global Handle Definitions ---
QueueHandle_t sensorQueue = NULL;
QueueHandle_t inputQueue = NULL;
QueueHandle_t alarmQueue = NULL;
QueueSetHandle_t displayQueueSet = NULL;

SemaphoreHandle_t serialMutex = NULL;
EventGroupHandle_t systemEventGroup = NULL;

// --- Initialization Functions ---

void rtos_sync_init(void) {
    systemEventGroup = xEventGroupCreate();
    if(systemEventGroup == NULL) {
        ESP_LOGE(RTOS_TAG, "Failed to create system event group!");
    } else {
        xEventGroupSetBits(systemEventGroup, EVENT_ACTIVE); // Initial System State
    }

    serialMutex = xSemaphoreCreateMutex();
    if(serialMutex == NULL) {
        ESP_LOGE(RTOS_TAG, "Failed to create serial mutex!");
    }
}

void rtos_queues_init(void) {
    if((sensorQueue = xQueueCreate(5, sizeof(SensorData))) == NULL) ESP_LOGE(RTOS_TAG, "Failed to create sensorQueue!");
    if((inputQueue = xQueueCreate(5, sizeof(uint8_t))) == NULL) ESP_LOGE(RTOS_TAG, "Failed to create InputTask Queue!");
    if((alarmQueue = xQueueCreate(5, sizeof(SensorData))) == NULL) ESP_LOGE(RTOS_TAG, "Failed to create alarmQueue!");

    displayQueueSet = xQueueCreateSet(5 + 5);
    if(displayQueueSet != NULL) {
        xQueueAddToSet(sensorQueue, displayQueueSet);
        xQueueAddToSet(inputQueue, displayQueueSet);
    } else {
        ESP_LOGE(RTOS_TAG, "Failed to create displayQueueSet!");
    }
}

void rtos_tasks_init(void) {
    // These functions are declared in rtos_objects.h, but the linker will 
    // find their actual while(true) loops over in main.cpp
    xTaskCreate(SensorTask, "DHT22 & LDR", 3072, NULL, 2, NULL);
    xTaskCreate(DisplayTask, "SSD1306 OLED Display", 4096, NULL, 1, NULL);
    xTaskCreate(InputTask, "Rotary Encoder", 2048, NULL, 3, NULL);
    xTaskCreate(AlarmTask, "Buzzer", 2048, NULL, 2, NULL);
    xTaskCreate(MotionTask, "PIR", 2048, NULL, 3, NULL);
}