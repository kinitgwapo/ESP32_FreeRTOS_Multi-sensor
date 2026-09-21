#ifndef MYPIR_SETUP_H
#define MYPIR_SETUP_H

// Libraries
#include <GPIO_Setup.h>
#include "myRTOS_Logic.h"
#include <esp_log.h>

// Function Declarations
void pirPin_Setup(uint64_t pin_mask);
void pir_evaluateMotion(bool status, bool *prevstatus);

#endif