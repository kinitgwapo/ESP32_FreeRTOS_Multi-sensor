#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

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

void app_main() {
    ESP_LOGI(SERIALMONITOR_TAG, "\nBCA152 FreeRTOS Multi-sensor\nSystem Starting...");

    // Configuration for handling tasks through FreeRTOS (FreeRTOS uses a pre-emptive scheduling on default)
    xTaskCreate(taskA, "Task A", 2048, NULL, 1, NULL);
    xTaskCreate(taskB, "Task B", 2048, NULL, 1, NULL);

    while(true) { // Free to use with FreeRTOS (Just avoid using delay that halts the CPU/Core/s)
        vTaskDelay(delay);
    }
}