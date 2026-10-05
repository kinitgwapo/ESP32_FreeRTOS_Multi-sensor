# ESP32 FreeRTOS Multi-sensor
A Real-Time Multi-sensor Room Monitoring System Project that utilizes the ESP-IDF and ESP-IDF FreeRTOS

# IN-PROGRESS (Currently in Inter-Task Communication)

## Project Overview

This ESP32 project is a smart monitoring and alert system designed to track motion and environmental conditions in real time. It collects real-time data from motion, temperature, humidity, and light sensors, displaying clear updates on an OLED screen. Users can navigate settings using a rotary dial, while a piezo buzzer provides instant sound alerts whenever the temperature exceeds the threshold.

## Features

- Real-Time Climate & Environmental Telemetry: Continuously logs ambient temperatures and humidity via the DHT22 Sensor, while tracking room lighting levels using an LDR Light Module.

- Motion Tracking & Smart Power Management: Utilizes a PIR Motion Sensor to detect movement in the surrounding area, maintaining the system in an Active state during occupancy and automatically triggering an Inactive state after 15 seconds of no detected motion.

- Interactive Multi-Screen OLED Display: Uses an SSD1306 OLED Display paired with a Rotary Encoder to let users manually cycle through dedicated telemetry views(temperature, humidity, light level, and motion status). The display automatically turns clears the screen in the Inactive state to conserve energy and prevent display burn-in, instantly waking up when motion is re-detected.

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

### Core Controller

- ESP32 Microcontroller Unit (MCU): 32-bit dual-core microcontroller operating at 240 MHz with integrated Wi-Fi and Bluetooth stacks, handling main application control loop timing, GPIO reading/interrupts, and I2C/SPI bus communications.

### Sensors & Input Modules

- Wokwi PIR Motion Sensor: Passive infrared sensor simulated in Wokwi with a 5-second active high-signal output delay (High pulse) upon detecting motion in its field of view, used to drive system transitions between Active and Inactive states.

- DHT22 (AM2302) Temperature & Humidity Sensor: Digital climate sensor utilizing a single-bus interface to collect environmental telemetry (Temperature range: -40°C to 80°C, Humidity range: 0-100%).

- LDR (Photoresistor) Sensor Module: Wokwi photoresistor element with an integrated built-in series 10kΩ resistor forming a voltage divider, capable of sensing light intensity across a wide dynamic range (0.1-100,000 lux).

- KY-040 Rotary Encoder Module: Incremental quadrature rotary encoder with an integrated tactile push button, providing digital rotation pulses for screen navigation and UI selection.

### Actuators & Output Devices

- SSD1306 OLED Display (128x64, I2C): 0.96-inch monochrome screen operating over the I2C protocol, responsible for displaying active sensor telemetry views and powering off/clearing during the 15-second inactivity timeout.

- Passive Piezoelectric Buzzer: Audio transducer driven via PWM output signals, current-limited by a series 220Ω resistor to trigger immediate audible alerts for high/low temperature safety thresholds.

## Pin Configuration

<img width="936" height="955" alt="Pin Configuration" src="https://github.com/user-attachments/assets/d74a2c3e-9c66-4b5c-9e27-d3dbe3daff7d" />

<div align="center">

| Component           | Pin             | Pin Mode                      |
| :------------------ | :-------------- | :---------------------------- |
| Buzzer              | Anode -> Pin 17 | PWM Output                    |
| LDR (Photoresistor) | A0 -> Pin 4     | ADC Input                     |
| PIR Sensor          | D -> Pin 16     | Normal Input                  |
| DHT22               | SDA -> Pin 23   | Bi-directional Open-Drain     |
| Rotary Encoder      | CLK -> Pin 18   | Positive-Edge Interrupt Input |
| Rotary Encoder      | DT -> Pin 19    | Normal Input                  |
| SSD1306 OLED        | SDA -> Pin 21   | I2C                           |
| SSD1306 OLED        | SCL -> Pin 22   | I2C                           |

</div>

## Task Design

| Task Name   | Responsibility/Role                                               | Execution Type    | Execution Period          | FreeRTOS Priority |
| :---------- | :---------------------------------------------------------------- | :---------------- | :------------------------ | :---------------- |
| InputTask   | Polls rotary encoder switch/dial inputs for UI navigation         | Periodic Polling  | 10ms                      | 3 (High)          |
| MotionTask  | Samples PIR sensor for physical motion activity                   | Periodic Polling  | 100ms                     | 3 (High)          |
| StateTask   | Manages Active/Inactive system state & 15s inactivity timer       | Event-Driven      | 100ms                     | 3 (High)          |
| SensorTask  | Samples DHT22 and LDR photoresistor telemetry                     | Periodic Sampling | 2000ms                    | 2 (Medium)        |
| AlarmTask   | Evaluates temperature thresholds and triggers Piezo buzzer alerts | Event-Driven      | Data Arrival              | 2 (Medium)        |
| DisplayTask | Renders telemetry screens and handles screen-off timeout          | Event-Driven      | Data Arrival/State Change | 1 (Low)           |

### Priority Configuration Rationale

In this project, there are 6 dedicated FreeRTOS tasks, each assigned a specific priority based on its execution model and real-time constraints:

- High Priority (Priority 3 - InputTask, MotionTask, StateTask): Tasks handling real-time inputs and core state management run at the highest priority. InputTask (10ms) and MotionTask (100ms) operate in polling mode to prevent missing fast hardware triggers (rotary encoder quadrature pulses or PIR output signals). StateTask (100ms) shares Priority 3 to immediately evaluate movement signals and process the 15-second inactivity timeout without preemption delay.

- Medium Priority (Priority 2 - SensorTask, AlarmTask): SensorTask acts as a Producer, fetching climate and light telemetry from the DHT22 and LDR photoresistor every 2 seconds. AlarmTask acts as a Consumer, waiting for queue items produced by SensorTask to evaluate safety limits (<= 18°C or >= 30°C). Although both tasks share Priority 2, they do not conflict because AlarmTask remains in a blocked state until new queue data arrives.

- Low Priority (Priority 1 - DisplayTask): DisplayTask acts as a visual Consumer. It remains blocked waiting for incoming telemetry or screen update notifications. Rendering graphics to the SSD1306 OLED over I2C is computationally non-critical, so running at Priority 1 ensures display updates never delay time-critical sensor sampling or user inputs.

### Preemption Risk Avoidance

If task priorities were improperly configured (for instance, if InputTask and MotionTask had unequal priorities), a higher-priority task becoming ready would preempt the lower-priority polling task mid-execution. This would stall the lower-priority task, causing dropped inputs or timing drift during signal evaluation.

### Isolated Task Profiles

1. InputTask
    - Purpose: Detects user interactions via the KY-040 Rotary Encoder for screen navigation.
    - Execution Pattern: Continuous polling mode via vTaskDelayUntil() every 10ms.
    - Internal Logic:
        1. Wakes up every 10ms.
        2. Reads digital logic levels of encoder CLK and DT pins through interrupt signal.
        3. Decodes quadrature signals to detect clockwise or counter-clockwise rotation.
        4. Updates internal UI navigation index.

2. MotionTask
    - Purpose: Performs low-level hardware sampling of the PIR motion sensor signal pin
    - Execution Pattern: Continuous polling mod every 100ms.
    - Internal Logic:
        1. Wakes up every 100ms.
        2. Reads digital input state from the PIR sensor pin.
        3. Flags physical movement detection (HIGH state).

3. StateTask
    - Purpose: Serves as the central system state machine, tracking active usage and inactivity timeouts.
    - Execution Pattern: Periodic logic evaluation tick every 100ms.
    - Internal Logic:
        1. Evaluates PIR motion status flags.
        2. If motion is active: Resets internal inactivity counter to 0s and maintains Active state.
        3. If motion is idle: Increments internal inactivity counter by 100ms.
        4. If inactivity counter reaches 15 seconds: Transitions system state to Inactive.

4. SensorTask
    - Purpose: Handles environmental telemetry acquisition from physical sensors.
    - Execution Pattern: Periodic sampling every 2000ms (2s).
    - Internal Logic:
        1. Wakes up every 2000ms.
        2. Queries DHT22 digital 1-Wire interface for temperature (°C) and humidity (%).
        3. Reads ADC analog voltage from LDR for light intensity (lux).
        4. Packs values into an internal telemetry memory struct.

5. AlarmTask
    - Purpose: Evaluates safety thresholds and operates the physical Piezo Buzzer hardware alert.
    - Execution Pattern: Event-driven/Blocked state.
    - Internal Logic:
        1. Remains blocked until new telemetry arrives.
        2. Evaluates temperature readings against bounds (<= 18°C or >= 30°C).
        3. Drives PWM frequency to trigger Piezo buzzer when bounds are breached; silences PWM when within normal bounds.
        4. Returns to blocked state.

6. DisplayTask
    - Purpose: Drives screen layout rendering on the SSD1306 OLED and manages screen sleep.
    - Execution Pattern: Event-driven/Blocked state.
    - Internal Logic:
        1. Remains blocked until UI navigation updates or system state changes occur.
        2. If system state is Inactive: Clears display buffer and powers down OLED screen controller via I2C.
        3. If system state is Active: Powers on OLED screen and renders active UI layout matching the current menu view.
        4. Returns to blocked state.

## Inter-Task Communication

| Communication Channel | Source (Producer) | Destination (Consumer) | FreeRTOS Object                  | Payload/Signal Structure                       |
| :-------------------- | :---------------- | :--------------------- | :------------------------------- | :--------------------------------------------- |
| Motion Signaling      | MotionTask        | StateTask, SensorTask  | Event Group (systemEventGroup)   | Bitmask (EVENT_MOTION)                         |
| System Power State    | StateTask         | DisplayTask            | Event Group (systemEventGroup)   | Bitmask (EVENT_ACTIVE)                         |
| UI Event Stream       | InputTask         | DisplayTask            | Queue (inputQueue)               | uint8_t (DisplayMode Enum Index)               |
| Telemetry Dispatch    | SensorTask        | DisplayTask, AlarmTask | Queues (sensorQueue, alarmQueue) | Struct (SensorData: {float, float, int, bool}) |
| Thread-safe Logging   | All Tasks         | UART Serial Monitor    | Mutex (serialMutex)              | Blocking Lock                                  |

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
