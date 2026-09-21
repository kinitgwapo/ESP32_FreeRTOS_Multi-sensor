#ifndef MYSYSTEMSTATE_LOGIC_H
#define MYSYSTEMSTATE_LOGIC_H

#include <stdint.h>
#include <stdbool.h>

bool evaluateSystemState(bool isActive, bool motionDetected, uint32_t elapsedMS, uint32_t timeoutMS);

#endif