#include <myBUZZER_Setup.hpp>

static AlarmState currentTemperatureMode;

AlarmState evaluateTemperature(float temperature) {
    if(temperature < 10.0f) {
        return currentTemperatureMode = AlarmState::LOW_TEMPERATURE;
    } else if(temperature > 35.0f) {
        return currentTemperatureMode = AlarmState::HIGH_TEMPERATURE;
    }
    return currentTemperatureMode = AlarmState::NORMAL;
}