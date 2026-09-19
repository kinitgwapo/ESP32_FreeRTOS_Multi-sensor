#include <myLDRModule_Setup.h>

adc_oneshot_unit_handle_t ldrHandle = NULL;

esp_err_t ldrmodule_ADC_oneshot_Setup(adc_unit_t adc_unit, adc_ulp_mode_t ulp_mode) {
    adc_oneshot_unit_init_cfg_t oneshotADC_Driver = {
        .unit_id = adc_unit,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ulp_mode
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&oneshotADC_Driver, &ldrHandle));
    return ESP_OK;
}

esp_err_t ldrmodule_ADC_oneshot_Channel(adc_channel_t channel) {
    adc_oneshot_chan_cfg_t oneshotADC_Channel = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(ldrHandle, channel, &oneshotADC_Channel));
    return ESP_OK;
}

esp_err_t ldrmodule_read_percentage(int *percentage, int *raw_value_out) {
    int raw_value = 0;
    esp_err_t err = adc_oneshot_read(ldrHandle, ADC_CHANNEL_0, &raw_value);
    
    if(err == ESP_OK) {
        // Range
        const int raw_dark = 4063;
        const int raw_bright = 32;

        // out-of-range Fix
        if(raw_value > raw_dark) raw_value = raw_dark;
        if(raw_value < raw_bright) raw_value = raw_bright;

        // Conversion & Inversion
        *percentage = ((raw_dark - raw_value) * 100) / (raw_dark - raw_bright);
        
        // Pass out the raw value for logging purposes if requested
        if(raw_value_out != NULL) {
            *raw_value_out = raw_value;
        }
    }
    return err;
}