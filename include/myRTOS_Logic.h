#ifndef MYRTOS_LOGIC_H
#define MYRTOS_LOGIC_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>

// --- System Data Structures ---
typedef struct {
    float dht22_temp;
    float dht22_humid;
    int lightLevel;
    bool motionDetected;
} SensorData;

// --- Event Group Bits ---
#define EVENT_ACTIVE BIT0
#define EVENT_MOTION BIT1
#define EVENT_ALARM  BIT2

// --- Extern Global Handles ---
extern QueueHandle_t sensorQueue;
extern QueueHandle_t inputQueue;
extern QueueHandle_t alarmQueue;
extern QueueSetHandle_t displayQueueSet;
extern SemaphoreHandle_t serialMutex;
extern EventGroupHandle_t systemEventGroup;

// --- Init Function Prototypes ---
void rtos_sync_init(void);
void rtos_queues_init(void);
void rtos_tasks_init(void);

// --- Task Function Declarations (Definitions in main.cpp) ---
void SensorTask(void *pvParameters);
void DisplayTask(void *pvParameters);
void InputTask(void *pvParameters);
void AlarmTask(void *pvParameters);
void MotionTask(void *pvParameters);

#endif // RTOS_OBJECTS_H