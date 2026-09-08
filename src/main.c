#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <myDHT22_Setup.h>

#include <myLDRModule_Setup.h>

const char *SERIALMONITOR_TAG = "MAIN APP"; // ESP_LOG Tagname
const TickType_t delay = 1000 / portTICK_PERIOD_MS; // Converting 1000ms to ticks (Used by vTaskDelay; vTaskDelay(pdMS_TO_TICKS(ms)) is a shortcut)

/*
    Tasks handled by FreeRTOS must have its own loop, else the task would run just once.
    Also, add a parameter even if your xTaskCreate param arg is NULL, compiling will
    throw an error if your task does not have an arg holder.

                                            REFRAIN FROM USING CPU/CORE HALTING TYPE OF DELAY:
            Blocking (RTOS type of Blocking)                                                    Non-blocking
    
    vTaskDelay(delay);                                                              taskYIELD();
        Pauses and checks other queued tasks                                            Pauses and checks other queued tasks
        Remains blocked even if there is no other tasks                                 Resumes immediately if no other tasks are found/ready 
*/

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

void taskC(void *pvParameters) {
    dhtPin_Setup(1ULL << GPIO_NUM_23);

    // DHT22 Data
    float temperature = 0.0f;
    float humidity = 0.0f;

    while(true) {
        esp_err_t DHT22 = dht22_start(&temperature, &humidity);
        if(DHT22 == ESP_OK) {
            ESP_LOGI(SERIALMONITOR_TAG, "Temp: %.1f°C, Humidity: %.1f%%", temperature, humidity);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void taskD(void *pvParameters) {
    extern adc_oneshot_unit_handle_t ldrHandle;

    while(true) {
        int raw_value = 0;
        esp_err_t LDR = adc_oneshot_read(ldrHandle, ADC_CHANNEL_0, &raw_value);

        if(LDR == ESP_OK) {
            // Range
            const int raw_dark = 4063;
            const int raw_bright = 32;

            // out-of-range Fix
            if(raw_value > raw_dark) raw_value = raw_dark;
            if(raw_value < raw_bright) raw_value = raw_bright;

            // Conversion & Inversion
            int percentage = ((raw_dark - raw_value) * 100) / (raw_dark - raw_bright);
            ESP_LOGI(SERIALMONITOR_TAG, "LDR Percentage: %d%% (Raw: %d)", percentage, raw_value);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main() {
    ESP_LOGI(SERIALMONITOR_TAG, "\nBCA152 FreeRTOS Multi-sensor\nSystem Starting...");

    // LDR Initial Config
    ldrmodule_ADC_oneshot_Setup(ADC_UNIT_2, ADC_ULP_MODE_DISABLE);
    ldrmodule_ADC_oneshot_Channel(ADC_CHANNEL_0);
    
    // Configuration for handling tasks through FreeRTOS (FreeRTOS uses a pre-emptive scheduling on default)
    xTaskCreate(taskA, "Task A", 2048, NULL, 1, NULL);
    xTaskCreate(taskB, "Task B", 2048, NULL, 1, NULL);
    xTaskCreate(taskC, "Task C", 2048, NULL, 1, NULL);
    xTaskCreate(taskD, "Task D", 2048, NULL, 1, NULL);

    while(true) { // Free to use with FreeRTOS (Just avoid using delay that halts the CPU/Core/s)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}