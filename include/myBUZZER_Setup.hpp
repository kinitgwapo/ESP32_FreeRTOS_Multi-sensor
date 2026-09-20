#ifndef MYBUZZER_SETUP_HPP
#define MYBUZZER_SETUP_HPP

// Libraries
extern "C" {
    #include <GPIO_Setup.h>
}
#include <driver/ledc.h>

enum class AlarmState {
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

// Function Declarations
void buzzerPin_Setup(uint64_t pin_mask);
AlarmState evaluateTemperature(float temperature);
void buzzer_Update(AlarmState currentResult);

#endif