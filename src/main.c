#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <myDHT22_Setup.h>

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

void app_main() {
    ESP_LOGI(SERIALMONITOR_TAG, "\nBCA152 FreeRTOS Multi-sensor\nSystem Starting...");
    

    // Configuration for handling tasks through FreeRTOS (FreeRTOS uses a pre-emptive scheduling on default)
    xTaskCreate(taskA, "Task A", 2048, NULL, 1, NULL);
    xTaskCreate(taskB, "Task B", 2048, NULL, 1, NULL);
    xTaskCreate(taskC, "Task C", 2048, NULL, 2, NULL); // Higher task to avoid getting Empty Value Data

    while(true) { // Free to use with FreeRTOS (Just avoid using delay that halts the CPU/Core/s)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}