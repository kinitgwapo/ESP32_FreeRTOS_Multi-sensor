#ifndef MYESP_SSD1306_I2C_SETUP_H
#define MYESP_SSD1306_I2C_SETUP_H

// Libraries
#include <ssd1306.h>
#include <driver/i2c_master.h>
#include "myRTOS_Logic.h" // For SensorData and DisplayMode (if included or forward declared)
#include "myROTARYENCODER_Setup.hpp" // For DisplayMode enum if needed
#include <stdio.h>

// Function Declarations
void myI2C_Config(void);
void display_update_screen(SensorData *data, uint8_t currentMode, bool isSystemActive, char *text, size_t textsize);
void unittest_display_update_screen(uint8_t currentMode, char *text, size_t textsize);

#endif