#include <myESP_SSD1306_I2C_Setup.h>

static i2c_master_bus_handle_t bus_handle = NULL;
static ssd1306_handle_t display_handle = NULL;

ssd1306_handle_t myI2C_Config(void) {
    i2c_master_bus_config_t bus_cfg = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = 21,
    .scl_io_num = 22,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    ssd1306_config_t cfg = {
        .bus = SSD1306_I2C,
        .width = 128,
        .height = 64,
        .iface.i2c = {
            .port = I2C_NUM_0,
            .addr = 0x3C,
            .rst_gpio = 17,
        },
    };
    ESP_ERROR_CHECK(ssd1306_new_i2c(&cfg, &display_handle));

    return display_handle;
}