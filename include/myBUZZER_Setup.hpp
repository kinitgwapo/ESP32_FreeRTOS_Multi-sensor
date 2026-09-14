#ifndef MYBUZZER_SETUP_HPP
#define MYBUZZER_SETUP_HPP

enum class AlarmState {
    NORMAL,
    LOW_TEMPERATURE,
    HIGH_TEMPERATURE
};

AlarmState evaluateTemperature(float temperature);

#endif