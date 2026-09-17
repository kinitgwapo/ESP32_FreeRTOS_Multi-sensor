#include <freertos/FreeRTOS.h>
#include <freertos/task.h> // FreeRTOS Task Manager
#include <freertos/queue.h> // FreeRTOS Queue Handle
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

// For Queueing Data safely
typedef struct {
    float dht22_temp;
    float dht22_humid;
    int lightLevel;
    bool motionDetected;
} SensorData;
QueueHandle_t sensorQueue; // Queue Handle

const char *SERIALMONITOR_TAG = "MAIN APP"; // ESP_LOG Tagname
EventGroupHandle_t systemEventGroup = NULL;
#define EVENT_ACTIVE BIT0
#define EVENT_MOTION BIT1
#define EVENT_ALARM BIT2

void SensorTask(void *pvParameters) {
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
        SensorData data;
        data.dht22_temp = (DHT22 == ESP_OK) ? temperature : 0.0f;
        data.dht22_humid = (DHT22 == ESP_OK) ? humidity : 0.0f;
        data.lightLevel = (LDR == ESP_OK) ? percentage : 0;
        data.motionDetected = (xEventGroupGetBits(systemEventGroup) & EVENT_MOTION); // PIR is now setup correctly

        //Send the package data to Queue
        xQueueSend(sensorQueue, &data, portMAX_DELAY);

        vTaskDelayUntil(&xLastWakeTime, frequency);
    }
}

void DisplayTask(void *pvParemeters) {
    SensorData receivedDataforDisplayTask = {0.0f, 0.0f, 0, false};
    ssd1306_handle_t displayhandle = myI2C_Config();
    char temporaryText[16];

    while(true) {
        xQueueReceive(sensorQueue, &receivedDataforDisplayTask, 0);

        if(xEventGroupGetBits(systemEventGroup) & EVENT_ACTIVE) {
            ssd1306_clear(displayhandle);
            ssd1306_draw_text(displayhandle, 0, 0, "ROOM MONITOR", true);

            switch(getCurrentDisplayMode()) {
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
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void InputTask(void *pvParameters) {
    rotaryEncoder_GPIO_Setup();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(10);
    uint8_t currentEncodeMode;
    uint8_t prevcurrentEncodeMode = 0;
    std::string textHolder = "";

    while(true) {
        currentEncodeMode = (uint8_t)checkRotaryEncoder();
        if(currentEncodeMode != prevcurrentEncodeMode) {
            switch(currentEncodeMode) {
                case 0: textHolder = "TEMPERATURE"; break;
                case 1: textHolder = "HUMIDITY"; break;
                case 2: textHolder = "LIGHT"; break;
                case 3: textHolder = "MOTION"; break;
            }
            ESP_LOGI(SERIALMONITOR_TAG, "Input Display Mode: %s", textHolder.c_str());
            prevcurrentEncodeMode = currentEncodeMode;
        }

        vTaskDelayUntil(&xLastWakeTime, frequency);
    }
}

void AlarmTask(void *pvParameters) {
    buzzerPin_Setup(1ULL << GPIO_NUM_17);
    while(true) {
        gpio_set_level(GPIO_NUM_17, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(GPIO_NUM_17, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
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
                ESP_LOGI(SERIALMONITOR_TAG, "Motion Detected! State changed: INACTIVE -> ACTIVE");
                xEventGroupSetBits(systemEventGroup, EVENT_ACTIVE);
            } else {
                ESP_LOGI(SERIALMONITOR_TAG, "Motion Detected!");
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
                ESP_LOGI(SERIALMONITOR_TAG, "Inactivity timeout (15s) reached! State changed: ACTIVE -> INACTIVE");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

extern "C" void app_main() {
    ESP_LOGI(SERIALMONITOR_TAG, "\nBCA152 FreeRTOS Multi-sensor\nSystem Starting...");
    if((sensorQueue = xQueueCreate(5, sizeof(SensorData))) == NULL) ESP_LOGE(SERIALMONITOR_TAG, "Failed to create sensorQueue!"); // Create Queue

    ldrmodule_ADC_oneshot_Setup(ADC_UNIT_2, ADC_ULP_MODE_DISABLE); ldrmodule_ADC_oneshot_Channel(ADC_CHANNEL_0); // LDR Initial Config

    systemEventGroup =  xEventGroupCreate();
    if(systemEventGroup == NULL) {
        ESP_LOGE(SERIALMONITOR_TAG, "Failed to create system event group!");
        return;
    }
    xEventGroupSetBits(systemEventGroup, EVENT_ACTIVE); // Initial System State
    
    // Configuration for handling tasks through FreeRTOS (FreeRTOS uses a pre-emptive scheduling on default)
    //xTaskCreate(SensorTask, "DHT22 & LDR", 3072, NULL, 2, NULL); // Upped stack depth for safety
    //xTaskCreate(DisplayTask, "SSD1306 OLED Display",4096 , NULL, 1, NULL); // Upped stack depth for safety
    //xTaskCreate(InputTask, "Rotary Encoder", 2048, NULL, 3, NULL);
    xTaskCreate(AlarmTask, "Buzzer", 2048, NULL, 2, NULL);
    //xTaskCreate(MotionTask, "PIR", 2048, NULL, 3, NULL);

    while(true) { // Free to use with FreeRTOS (Just avoid using delay that halts the CPU/Core/s)
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}