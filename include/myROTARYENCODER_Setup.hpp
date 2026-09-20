#ifndef MYROTARYENCODER_SETUP_H
#define MYROTARYENCODER_SETUP_H

// Macros
#define CLK_PIN GPIO_NUM_18
#define DT_PIN GPIO_NUM_19

// Libraries
extern "C" {
    #include <GPIO_Setup.h>
}
#include <esp_attr.h> // IRAM
#include "myRTOS_Logic.h"
#include <esp_log.h>

// Variables
enum class DisplayMode {
    TEMPERATURE,
    HUMIDITY,
    LIGHT,
    MOTION
};

// Function Declarations
void rotaryEncoder_GPIO_Setup(void);
DisplayMode checkRotaryEncoder(void);
void rotaryEncoder_SendData(uint8_t *currentEncodeMode, uint8_t *prevcurrentEncodeMode);

#endif