#ifndef GPIO_SETUP_H
#define GPIO_SETUP_H

// Libraries
#include <driver/gpio.h>



// Function Declarations
/*
    Arg "pin_mask" can accept multiple GPIOs, but must be written as [1ULL << (GPIO_NUM_#)] or [(1ULL << GPIO_NUM_#) | (1ULL << GPIO_NUM_#)]

    The parameters are based on the "driver/gpio.h" library, please refer to that library if you wish to understand how this function works
*/
void gpio_init(gpio_config_t *config, uint64_t pin_mask, gpio_mode_t gpio_mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t gpio_set_intr_type);



#endif