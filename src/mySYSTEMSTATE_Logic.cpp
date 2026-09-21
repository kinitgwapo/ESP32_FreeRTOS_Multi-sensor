#include "mySYSTEMSTATE_Logic.h"

bool evaluateSystemState(bool isActive, bool motionDetected, uint32_t elapsedMS, uint32_t timeoutMS) {
    if(isActive) {
        if(elapsedMS > timeoutMS) {
            return false;
        }
        return true;
    } else {
        if(motionDetected) {
            return true;
        }
        return false;
    }
}