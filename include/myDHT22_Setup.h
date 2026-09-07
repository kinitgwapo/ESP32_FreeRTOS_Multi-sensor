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
*/

// Function Declarations
void dhtPin_Setup(uint64_t pin_mask); // Pass (1ULL << GPIO_NUM_4) instead of GPIO_NUM_4
esp_err_t dht22_start(float *temp, float *humid); // returns float value of temperature and humidity

#endif