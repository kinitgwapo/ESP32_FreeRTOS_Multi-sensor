#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

const char *SERIALMONITOR_TAG = "MAIN APP";
const TickType_t delay = 500 / portTICK_PERIOD_MS;

void app_main() {
    ESP_LOGI(SERIALMONITOR_TAG, "\nBCA152 FreeRTOS Multi-sensor\nSystem Starting...");

    while(true) {
        vTaskDelay(delay);
    }
}


/* Note:
    Blocking (RTOS type of Blocking)
    vTaskDelay(delay);
    Pauses and checks other queued tasks. Remains blocked even if there is no other tasks

    Non-blocking
    taskYIELD();
    Pauses and checks other queued tasks. Resumes immediately if no other tasks are found/ready
*/