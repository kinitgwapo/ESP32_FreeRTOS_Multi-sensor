# ESP32 FreeRTOS Multi-sensor
A Real-Time Multi-sensor Room Monitoring System Project that utilizes the ESP-IDF and ESP-IDF FreeRTOS


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

# References
DHT22 Hardware Design: https://components101.com/sites/default/files/component_datasheet/DHT22%20Sensor%20Datasheet.pdf & https://components101.com/sensors/dht22-pinout-specs-datasheet

DHT22 Wokwi Library: https://github.com/beegee-tokyo/DHTesp

LDR Module Wokwi: https://docs.wokwi.com/parts/wokwi-photoresistor-sensor

SSD1306 Display Driver Library: https://github.com/Chill-Sam/esp-ssd1306 & https://docs.wokwi.com/parts/board-ssd1306

Markdown Format: https://www.w3schools.com/tools/tool_markdown_table.php & https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#tables
