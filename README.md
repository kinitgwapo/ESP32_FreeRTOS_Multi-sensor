# ESP32 FreeRTOS Multi-sensor
A Real-Time Multi-sensor Room Monitoring System Project that utilizes the ESP-IDF and ESP-IDF FreeRTOS

# IN-PROGRESS

## Project Overview

This ESP32 project is a smart monitoring and alert system designed to track motion and environmental conditions in real time. It collects real-time data from motion, temperature, humidity, and light sensors, displaying clear updates on an OLED screen. Users can navigate settings using a rotary dial, while a piezo buzzer provides instant sound alerts whenever the temperature exceeds the threshold.

## Features

- Real-Time Climate & Environmental Telemetry: Continuously logs ambient temperatures and humidity via the DHT22 Sensor, while tracking room lighting levels using an LDR Light Module.

- Motion Tracking: Utilizes a PIR Motion Sensor to detect movement within the surrounding area and update live motion status.

- Display Interface: Uses an SSD1306 OLED Display paired with a Rotary Encoder to let users manually cycle through dedicated views for temperature, humidity, light level, and motion status.

- Automated Temperature Safety Alarm: Employs a Piezo Buzzer to trigger immediate audible alerts whenever measured temperatures drift outside safe operational bounds (<= 18°C or >= 30°C).

## Learning Objectives

#### Development Tools & Virtual Prototyping

- VSCode & PlatformIO Setup - Manage ESP32 builds, board settings, and libraries inside a clean, modern development workspace.

- Hardware Simulation with Wokwi - Testing and debugging circuit logic virtually in a browser before assembling physical wiring.

#### Smooth Multi-Tasking with FreeRTOS

- FreeRTOS Queues - Passing Sensor readings and dial clicks safely between background tasks so the screen and sensors never freeze or lag.

- Mutexes & Semaphores - Preventing display glitches when multiple tasks share a single resource concurrently, while triggering instant sound alerts across tasks.

#### Smart Code Organization & State Logic

- Centralized State Controller - Handling display screen switches and alert modes with a structured state machine instead of long, messy if-else blocks.

- Clean Folder & Driver Layout - Separating internal code from external driver files inside structured 'src/' and 'include/' folders for easy, reliable builds.

#### Automated Testing & Code Quality

- Unit Testing with Unity - Verifying core project rules automatically using PlatformIO Unit tests, without needing physical sensors connected.

- Static Analysis with Cppcheck - Filtering out background system noise so automated code checks highlight only real bugs and memory issues.

## System Architecture

<img width="1349" height="717" alt="ESP32 Multi-Sensor Room Monitoring System Architecture" src="https://github.com/user-attachments/assets/9bd7f10a-396e-42b9-9834-6288751798be" />

## FreeRTOS Architecture

<img width="1390" height="744" alt="FreeRTOS Architecture" src="https://github.com/user-attachments/assets/3b01ae4e-d0f2-403b-a82f-42bbe6415733" />

## Hardware / Simulated Components



## Pin Configuration

<img width="936" height="955" alt="Pin Configuration" src="https://github.com/user-attachments/assets/d74a2c3e-9c66-4b5c-9e27-d3dbe3daff7d" />

## Task Design



## Inter-Task Communication



## State Machine

<img width="1196" height="658" alt="State Machine" src="https://github.com/user-attachments/assets/5462e85c-784a-4f79-80a6-7d906f2aaf0b" />

## Repository Structure



## Getting Started



## Building the Project



## Running the Wokwi Simulation



## Unit Testing



## Static Code Analysis

| Finding                    | File/Line       | Cause                            | Resolution          |
| :------------------------: | :-------------: | :------------------------------: | :-----------------: |
| Unnecessary Variable Scope | src\main.cpp:90 | variable Scope (status)          | Moved to while loop |
| Assigned Value never used  | src\main.cpp:90 | unread Variable (status = false) | Same as above       |

src\main.cpp:90: [low:style] The scope of the variable 'status' can be reduced. [variableScope]
src\main.cpp:90: [low:style] Variable 'status' is assigned a value that is never used. [unreadVariable]

## Functional Verification



## Engineering Decisions



## Limitations



## Future Improvements



## References and Acknowledgments

DHT22 Hardware Design: https://components101.com/sites/default/files/component_datasheet/DHT22%20Sensor%20Datasheet.pdf & https://components101.com/sensors/dht22-pinout-specs-datasheet

DHT22 Wokwi Library: https://github.com/beegee-tokyo/DHTesp

LDR Module Wokwi: https://docs.wokwi.com/parts/wokwi-photoresistor-sensor

SSD1306 Display Driver Library: https://github.com/Chill-Sam/esp-ssd1306 & https://docs.wokwi.com/parts/board-ssd1306

PIR Sensor Wokwi: https://docs.wokwi.com/parts/wokwi-pir-motion-sensor

Markdown Format: https://www.w3schools.com/tools/tool_markdown_table.php & https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#tables















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
