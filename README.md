# ESP32 FreeRTOS Multi-sensor
A Real-Time Multi-sensor Room Monitoring System Project that utilizes the ESP-IDF and ESP-IDF FreeRTOS


# Note:
## Dummy Task's FreeRTOS Configuration
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

# References
DHT22: https://components101.com/sites/default/files/component_datasheet/DHT22%20Sensor%20Datasheet.pdf & https://components101.com/sensors/dht22-pinout-specs-datasheet
Markdown Format: https://www.w3schools.com/tools/tool_markdown_table.php & https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#tables
