#include <myESP_SSD1306_I2C_Setup.h>


static i2c_master_bus_handle_t bus_handle = NULL;
static ssd1306_handle_t display_handle = NULL;

void myI2C_Config(void) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    i2c_master_bus_config_t bus_cfg = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = GPIO_NUM_21,
    .scl_io_num = GPIO_NUM_22,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7
    };
    bus_cfg.flags.enable_internal_pullup = true;
    i2c_new_master_bus(&bus_cfg, &bus_handle);

    ssd1306_config_t cfg = {
        .bus = SSD1306_I2C,
        .width = 128,
        .height = 64
    };
    cfg.iface.i2c = {
        .port = I2C_NUM_0,
        .addr = 0x3C
    },
    ssd1306_new_i2c(&cfg, &display_handle);
    #pragma GCC diagnostic pop
}

void display_update_screen(SensorData *data, uint8_t currentMode, bool isSystemActive) {
    char temporaryText[16];

    if(isSystemActive) {
        ssd1306_clear(display_handle);
        ssd1306_draw_text(display_handle, 0, 0, "ROOM MONITOR", true);

        switch((DisplayMode)currentMode) {
            case DisplayMode::TEMPERATURE:
                ssd1306_draw_text(display_handle, 0, 20, "TEMPERATURE", true);
                snprintf(temporaryText, sizeof(temporaryText), "%.1f C", data->dht22_temp);
                break;
            case DisplayMode::HUMIDITY:
                ssd1306_draw_text(display_handle, 0, 20, "HUMIDITY", true);
                snprintf(temporaryText, sizeof(temporaryText), "%.1f RH", data->dht22_humid);     
                break;
            case DisplayMode::LIGHT:
                ssd1306_draw_text(display_handle, 0, 20, "LIGHT", true);
                snprintf(temporaryText, sizeof(temporaryText), "%d", data->lightLevel);
                break;
            case DisplayMode::MOTION:
                ssd1306_draw_text(display_handle, 0, 20, "MOTION", true);
                snprintf(temporaryText, sizeof(temporaryText), "%s", data->motionDetected ? "DETECTED" : "CLEAR");
                break;
        }
        ssd1306_draw_text(display_handle, 0, 32, temporaryText, true);
        ssd1306_display(display_handle);
    } else {
        ssd1306_clear(display_handle);
        ssd1306_display(display_handle);
    }
}