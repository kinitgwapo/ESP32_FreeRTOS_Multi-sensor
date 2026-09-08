#ifndef MYLDRMODULE_SETUP_H
#define MYLDRMODULE_SETUP_H

// Libraries
#include <esp_adc/adc_oneshot.h>

/*
    This Wokwi LDR Module uses a fixed 10kΩ resistor.

    Circuit Setup:

             GPIO
              |
    LDR -> Midpoint -> Fixed-Resistor -> LDR-Gnd

    Lux-Raw Range:
    0.1lux = 32 raw value
    100klux = 4063 raw value

    The raw value is inverted because the GPIO reads a barely-dimmed LDR as 4063, which should actually be 32 for human intuition.
*/


// Function Declarations
esp_err_t ldrmodule_ADC_oneshot_Setup(adc_unit_t adc_unit, adc_ulp_mode_t ulp_mode);
esp_err_t ldrmodule_ADC_oneshot_Channel(adc_channel_t channel);

#endif