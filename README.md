# ESP32 FreeRTOS Multi-sensor
A Real-Time Multi-sensor Room Monitoring System Project that utilizes the ESP-IDF and ESP-IDF FreeRTOS

# IN-PROGRESS
### Unit Testing - Done (On UT-SCA_Branch Branch)
### Static Code Analysis - Pending (To be implemented with the UT-SCA_Branch Branch)

# Note:
### Dummy Task's FreeRTOS Configuration
| Task Name | FreeRTOS (Delay) | Priority # |
| :-------- | :--------------: | ---------: |
| Task A    | 1 Second         | 1          |
| Task B    | 1 Second         | 1          |

### Example of Execution
| Task Name  | Task Status | Description           | Scheduler            |
| :--------- | :---------: | :-------------------: | :------------------: |
| Task A & B | Ready       | Waiting for Scheduler |                      |
| Task B     | Blocked     | Waiting for 1s        | Round-Robin Decision |
| Task A     | Running     | Executing             |                      |
| Task A     | Blocked     | Waiting for 1s        |                      |
| Task B     | Ready       | Waiting for Scheduler |                      |
| Task B     | Running     | Executing             |                      |
| Task B     | Blocked     | Waiting for 1s        |                      |
| Task A     | Ready       | Waiting for Scheduler |                      |

The execution frequency for each task is 1hz, or 1 task per second.

### vTaskDelay() & vTaskDelayUntil() Difference
When vTaskDelay is executed, the current task is blocked for a specific time, allowing other task to work. Once the specific time is reached, the task becomes ready and is queued to the scheduler awaiting for signal.

vTaskDelayUntil has the same principle as vTaskDelay, but with the precedented absolute tick/time interval. If your task finishes after 5ms and starts every 100ms, instead of waiting for 100ms + 5ms because it finished quickly, vTaskDelayUntil sleeps the task for 95ms from the absolute starting tick and time it took to finish the task, removing the drift problem unlike vTaskDelay.

### Mutex Explanation
Mutex (Mutual Exclusion) is a method that prevents a certain resource from being manipulated at the same time, which causes a problem such as corrupted data, garbled output when the
program is designed to be running tasks concurrently. The ESP_LOGI function is one of the examples of a ESPLOG API that shares its one resource to multiple tasks. Having
multiple tasks be able to manipulate the data simultaneously causes a disaster in UART Serial transmission buffer, leaving the Serial Monitor output a garbage or deformed transmitted data. In this project, SensorTask, InputTask, and MotionTask are the competing tasks that uses ESP_LOGI concurrently.

### Task Priorities
In this Project, there are currently 5 tasks with different purposes.

| Task Identity | DisplayTask | AlarmTask | SensorTask | InputTask | MotionTask |
| :-----------: | :---------: | :-------: | :--------: | :-------: | :--------: |
| Task Priority | 1           | 2         | 2          | 3         | 3          |

Each task have their own priority based on how they function. User Input Tasks such as, "InputTask & MotionTask" are set as the highest priority from all tasks because
they're designed to be running in polling mode, which involves losing some input before the task begins checking once again. Input runs every 10ms, while Motion starts every 100ms.

For the three remaining tasks, "DisplayTask & AlarmTask" acts as a consumer, and "SensorTask" is a producer. The SensorTask fetches data from DHT22 & LDR (Photoresistor) every
2s, while AlarmTask and DisplayTask only waits for the upcoming items from the FreeRTOS Queue that is provided by the consumer (SensorTask). Although the SensorTask and AlarmTask have the same priority, they do not necessarily become a conflict because both AlarmTask and DisplayTask waits for Queue data and if the SensorTask interrupts for some reason, the data fetched by AlarmTask and DisplayTask will not be affected.

If task priority were set improperly, such as an InputTask & MotionTask not having the same priority, every time the highest priority is ready while the lower priority is currently running, the highest priority task interrupts the lower priority, leaving the lower priority task paused and lose its progress due to data fetch transferring corruption because the task looses a portion of its polling time while the higher one is stable.

# References
DHT22 Hardware Design: https://components101.com/sites/default/files/component_datasheet/DHT22%20Sensor%20Datasheet.pdf & https://components101.com/sensors/dht22-pinout-specs-datasheet

DHT22 Wokwi Library: https://github.com/beegee-tokyo/DHTesp

LDR Module Wokwi: https://docs.wokwi.com/parts/wokwi-photoresistor-sensor

SSD1306 Display Driver Library: https://github.com/Chill-Sam/esp-ssd1306 & https://docs.wokwi.com/parts/board-ssd1306

Markdown Format: https://www.w3schools.com/tools/tool_markdown_table.php & https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#tables
