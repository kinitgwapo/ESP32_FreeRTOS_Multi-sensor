#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <freertos/queue.h>

#include <myDHT22_Setup.h>

#include <myLDRModule_Setup.h>

// For Queueing Data safely
struct SensorData {
    float dht22_temp;
    float dht22_humid;
    int lightLevel;
    bool motionDetected;
};
QueueHandle_t sensorQueue; // Queue Handle

const char *SERIALMONITOR_TAG = "MAIN APP"; // ESP_LOG Tagname
const TickType_t delay = 1000 / portTICK_PERIOD_MS; // Converting 1000ms to ticks (Used by vTaskDelay; vTaskDelay(pdMS_TO_TICKS(ms)) is a shortcut)

void taskA(void *pvParameters) {
    while(true) {
        ESP_LOGI(SERIALMONITOR_TAG, "Task A Running");
        vTaskDelay(delay);
    }
}

void taskB(void *pvParameters) {
    while(true) {
        ESP_LOGI(SERIALMONITOR_TAG, "Task B Running");
        vTaskDelay(delay);
    }
}

void dht22_ldr_Task(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(2000);
    extern adc_oneshot_unit_handle_t ldrHandle;
    dhtPin_Setup(1ULL << GPIO_NUM_23);

    while(true) {
        float temperature = 0.0f;
        float humidity = 0.0f;
        esp_err_t DHT22 = dht22_start(&temperature, &humidity);
        if(DHT22 == ESP_OK) {
            ESP_LOGI(SERIALMONITOR_TAG, "Temp: %.1f°C, Humidity: %.1f%%", temperature, humidity);
        }

        int raw_value = 0;
        esp_err_t LDR = adc_oneshot_read(ldrHandle, ADC_CHANNEL_0, &raw_value);
        int percentage = 0;
        if(LDR == ESP_OK) {
            // Range
            const int raw_dark = 4063;
            const int raw_bright = 32;

            // out-of-range Fix
            if(raw_value > raw_dark) raw_value = raw_dark;
            if(raw_value < raw_bright) raw_value = raw_bright;

            // Conversion & Inversion
            percentage = ((raw_dark - raw_value) * 100) / (raw_dark - raw_bright);
            ESP_LOGI(SERIALMONITOR_TAG, "LDR Percentage: %d%% (Raw: %d)", percentage, raw_value);
        }

        // Defensive Packaged queue data reading
        struct SensorData data;
        data.dht22_temp = (DHT22 == ESP_OK) ? temperature : 0.0f;
        data.dht22_humid = (DHT22 == ESP_OK) ? humidity : 0.0f;
        data.lightLevel = (LDR == ESP_OK) ? percentage : 0;
        data.motionDetected = false; // Placeholder until PIR is added

        //Send the package data to Queue
        xQueueSend(sensorQueue, &data, portMAX_DELAY);

        vTaskDelayUntil(&xLastWakeTime, frequency);
    }
}

void app_main() {
    ESP_LOGI(SERIALMONITOR_TAG, "\nBCA152 FreeRTOS Multi-sensor\nSystem Starting...");

    sensorQueue = xQueueCreate(5, sizeof(struct SensorData));

    // LDR Initial Config
    ldrmodule_ADC_oneshot_Setup(ADC_UNIT_2, ADC_ULP_MODE_DISABLE);
    ldrmodule_ADC_oneshot_Channel(ADC_CHANNEL_0);
    
    // Configuration for handling tasks through FreeRTOS (FreeRTOS uses a pre-emptive scheduling on default)
    xTaskCreate(taskA, "Task A", 2048, NULL, 1, NULL);
    xTaskCreate(taskB, "Task B", 2048, NULL, 1, NULL);
    xTaskCreate(dht22_ldr_Task, "SensorTask", 3072, NULL, 1, NULL); // Upped stack depth for safety

    while(true) { // Free to use with FreeRTOS (Just avoid using delay that halts the CPU/Core/s)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}