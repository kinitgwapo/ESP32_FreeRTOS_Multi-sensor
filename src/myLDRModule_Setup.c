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