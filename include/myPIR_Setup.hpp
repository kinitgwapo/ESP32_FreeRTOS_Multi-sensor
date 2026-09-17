#ifndef MYPIR_SETUP_H
#define MYPIR_SETUP_H

// Libraries
extern "C" {
    #include <GPIO_Setup.h>
}


enum class SystemState {
    ACTIVE,
    INACTIVE
};

extern SystemState currentSystemState;

// Function Declarations
void pirPin_Setup(uint64_t pin_mask);

#endif