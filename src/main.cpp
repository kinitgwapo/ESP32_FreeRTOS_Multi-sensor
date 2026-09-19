#include "myRTOS_Logic.h"
#include <esp_log.h> // Logging Functions
#include "myROTARYENCODER_Setup.hpp" // Poll Rotary input
#include "myBUZZER_Setup.hpp" // Buzzer Temperature Configuration
#include <string> // For String Printing
extern "C" {
    #include "myDHT22_Setup.h" // Process DHT22 Data
    #include "myLDRModule_Setup.h" // Process LDR Data
    #include <ssd1306.h> // OLED Display Driver
    #include "myESP_SSD1306_I2C_Setup.h" // OLED Display I2C Automated Driver Setup
    #include "myPIR_Setup.h" // Pir pin Setup
}

const char *SERIALMONITOR_TAG = "MAIN APP"; // ESP_LOG Tagname

void SensorTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    dhtPin_Setup(1ULL << GPIO_NUM_23);

    while(true) {
        float temperature = 0.0f;
        float humidity = 0.0f;
        esp_err_t DHT22 = dht22_start(&temperature, &humidity);
        if(DHT22 == ESP_OK) {
            if(xSemaphoreTake(serialMutex, portMAX_DELAY) == pdPASS) {
                ESP_LOGI(SERIALMONITOR_TAG, "Temp: %.1f°C, Humidity: %.1f%%", temperature, humidity);
                xSemaphoreGive(serialMutex);
            }     
        }

        int raw_value = 0;
        int percentage = 0;
        esp_err_t LDR = ldrmodule_read_percentage(&percentage, &raw_value);
        if(LDR == ESP_OK) {
            // Range
            if(xSemaphoreTake(serialMutex, portMAX_DELAY) == pdPASS) {
                ESP_LOGI(SERIALMONITOR_TAG, "LDR Percentage: %d%% (Raw: %d)", percentage, raw_value);
                xSemaphoreGive(serialMutex);
            }
        }

        // Send everything through the single RTOS interface
        rtos_send_sensor_data(DHT22, temperature, humidity, LDR, percentage);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2000));
    }
}

void DisplayTask(void *pvParameters) {
    SensorData receivedDataforDisplayTask = {0.0f, 0.0f, 0, false};
    uint8_t currentMode = 0;

    ssd1306_handle_t displayhandle = myI2C_Config();
    char temporaryText[16];

    while(true) {
        QueueSetMemberHandle_t activeMember = xQueueSelectFromSet(displayQueueSet, portMAX_DELAY);

        if(activeMember == sensorQueue) xQueueReceive(sensorQueue, &receivedDataforDisplayTask, 0);
        if(activeMember == inputQueue) xQueueReceive(inputQueue, &currentMode, 0);

        if(xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE) {
            ssd1306_clear(displayhandle);
            ssd1306_draw_text(displayhandle, 0, 0, "ROOM MONITOR", true);

            switch((DisplayMode)currentMode) {
                case DisplayMode::TEMPERATURE:
                    ssd1306_draw_text(displayhandle, 0, 20, "TEMPERATURE", true);
                    snprintf(temporaryText, sizeof(temporaryText), "%.1f C", receivedDataforDisplayTask.dht22_temp);
                    break;
                case DisplayMode::HUMIDITY:
                    ssd1306_draw_text(displayhandle, 0, 20, "HUMIDITY", true);
                    snprintf(temporaryText, sizeof(temporaryText), "%.1f RH", receivedDataforDisplayTask.dht22_humid);     
                    break;
                case DisplayMode::LIGHT:
                    ssd1306_draw_text(displayhandle, 0, 20, "LIGHT", true);
                    snprintf(temporaryText, sizeof(temporaryText), "%d", receivedDataforDisplayTask.lightLevel);
                    break;
                case DisplayMode::MOTION:
                    ssd1306_draw_text(displayhandle, 0, 20, "MOTION", true);
                    snprintf(temporaryText, sizeof(temporaryText), "%s", receivedDataforDisplayTask.motionDetected ? "DETECTED" : "CLEAR");
                    break;
            }
            ssd1306_draw_text(displayhandle, 0, 32, temporaryText, true);
            ssd1306_display(displayhandle);
        } else {
            ssd1306_clear(displayhandle);
            ssd1306_display(displayhandle);
        }
    }
}

void InputTask(void *pvParameters) {
    rotaryEncoder_GPIO_Setup();
    uint8_t currentEncodeMode;
    uint8_t prevcurrentEncodeMode = 0;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(10);

    while(true) {
        currentEncodeMode = (uint8_t)checkRotaryEncoder();
        if(currentEncodeMode != prevcurrentEncodeMode) {
            xQueueSend(inputQueue, &currentEncodeMode, 0);
            if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                ESP_LOGI(SERIALMONITOR_TAG, "Input Produced Mode: %d", currentEncodeMode);
                xSemaphoreGive(serialMutex);
            }
            prevcurrentEncodeMode = currentEncodeMode;
        }

        vTaskDelayUntil(&xLastWakeTime, frequency);
    }
}

void AlarmTask(void *pvParameters) {
    SensorData receivedDataforAlarmTask = {0.0f, 0.0f, 0, false};
    buzzerPin_Setup(1ULL << GPIO_NUM_17);

    while(true) {
        if(xQueueReceive(alarmQueue, &receivedDataforAlarmTask, portMAX_DELAY) == pdPASS) {
            AlarmState currentResult = evaluateTemperature(receivedDataforAlarmTask.dht22_temp);
            switch(currentResult) {
                case AlarmState::NORMAL:
                    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
                    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
                    break;
                case AlarmState::LOW_TEMPERATURE:
                case AlarmState::HIGH_TEMPERATURE:
                    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 127);
                    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
                    break;
            }
        }
    }
}

void MotionTask(void *pvParameters) {
    bool status = false;
    bool prevstatus = false;
    pirPin_Setup(1ULL << GPIO_NUM_16);

    TickType_t lastMotionTick = xTaskGetTickCount();
    const TickType_t inactivityTimeout = pdMS_TO_TICKS(15000);

    while(true) {
        status = gpio_get_level(GPIO_NUM_16);

        if(status == true && prevstatus == false) {
            if(!(xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE)) {
                if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                    ESP_LOGI(SERIALMONITOR_TAG, "Motion Detected! State changed: INACTIVE -> ACTIVE");
                    xSemaphoreGive(serialMutex);
                }
                xEventGroupSetBits(systemEventGroup, EVENT_ACTIVE);
            } else {
                if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                    ESP_LOGI(SERIALMONITOR_TAG, "Motion Detected!");
                    xSemaphoreGive(serialMutex);
                }
            }
        }

        if(status == true) {
            xEventGroupSetBits(systemEventGroup, EVENT_MOTION);
            lastMotionTick = xTaskGetTickCount();
        } else {
            xEventGroupClearBits(systemEventGroup, EVENT_MOTION);
        }

        prevstatus = status;

        if(xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE) {
            if((xTaskGetTickCount() - lastMotionTick) > inactivityTimeout) {
                xEventGroupClearBits(systemEventGroup, EVENT_ACTIVE);
                if(xSemaphoreTake(serialMutex, portMAX_DELAY)) {
                    ESP_LOGI(SERIALMONITOR_TAG, "Inactivity timeout (15s) reached! State changed: ACTIVE -> INACTIVE");
                    xSemaphoreGive(serialMutex);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

extern "C" void app_main() {
    ESP_LOGI(SERIALMONITOR_TAG, "\nBCA152 FreeRTOS Multi-sensor\nSystem Starting...");

    // Hardware LDR Initial Config
    ldrmodule_ADC_oneshot_Setup(ADC_UNIT_2, ADC_ULP_MODE_DISABLE);
    ldrmodule_ADC_oneshot_Channel(ADC_CHANNEL_0);

    rtos_sync_init();
    rtos_queues_init();
    rtos_tasks_init();

    while(true) { // Free to use with FreeRTOS (Just avoid using delay that halts the CPU/Core/s)
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}