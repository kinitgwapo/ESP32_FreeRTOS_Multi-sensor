#ifndef MYDHT22_SETUP_H
#define MYDHT22_SETUP_H

// Libraries
#include <GPIO_Setup.h>

#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_rom_sys.h>

#include <esp_log.h>

/*
    Connect a Capacitor between VDD and Gnd for Actual Hardware to minimize noise. (100 nF non-polarized Capacitor)
    Have the data pin as pull-up resistor, either external or internal setup. (4.7kΩ-10kΩ)

    Data pin must be connected to a Bi-directional GPIO pin because it sends and receives data both from MCU and DHT22.

    It is a must for calling receive_DHT22_Signal() to enter a critical section and the function is in Internal RAM to
    avoid crashes, cache misses, leading to zero value temp and humid.


    Timing Diagram:
        Starting Signal
            | MCU SIGNAL   | MCU SIGNAL | MCU SIGNAL  | DHT22 Pull Delay | DHT22 SIGNAL | DHT22 SIGNAL |
            | :----------: | :--------: | :---------: | :--------------: | :----------: | :----------: |
            | Initial-HIGH | (18ms)-LOW | (40µs)-HIGH | (50µs)-HIGHtoLOW | (80µs)-LOW   | (80µs)-HIGH  |

        40-bits of Data in-order (After Starting Signal)
            | INT REL HUMID | DECIMAL REL HUMID | INT TEMP | DECIMAL TEMP | CHEKSUM |
            | :-----------: | :---------------: | :------: | :----------: | :-----: |
            | 8-bit         | 8-bit             | 8-bit    | 8-bit        | 8-bit   |

        High Pulse Logic (28µs HIGH = Low Logic) (70µs HIGH = High Logic)

        Data Signal (Low Logic) (After Starting Signal)
            | DHT22 SIGNAL | DHT22 SIGNAL |
            | :----------: | :----------: |
            | 50µs-LOW     | 28µs-HIGH    |
            | Data Signal  | Low Logic    |

        Data Signal (High Logic) (After Starting Signal)
            | DHT22 SIGNAL | DHT22 SIGNAL |
            | :----------: | :----------: |
            | 50µs-LOW     | 70µs-HIGH    |
            | Data Signal  | High Logic   |
*/

// Function Declarations
void dhtPin_Setup(uint64_t pin_mask); // Pass (1ULL << GPIO_NUM_4) instead of GPIO_NUM_4
esp_err_t dht22_start(float *temp, float *humid); // returns float value of temperature and humidity

#endif