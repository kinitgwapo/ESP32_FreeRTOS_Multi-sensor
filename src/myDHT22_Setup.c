#include <myDHT22_Setup.h>

static gpio_config_t DHT22_Pin;
static uint8_t pin_num;
static uint8_t timeout;

// IRAM_ATTR puts function in Internal RAM (Instruction RAM) to avoid cache misses, aiding the critical section
static esp_err_t IRAM_ATTR receive_DHT22_Signal(float *temp, float *humid) {
    uint8_t data[5] = {0};
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; // Declaration of Spinlock 

    portENTER_CRITICAL(&mux); // Disables core local interrupts
    for(int i = 0; i < 40; i++) { // Iterate 40 times for 40-bits of data
        timeout = 0;
        while(gpio_get_level(pin_num) == 0) {
            esp_rom_delay_us(1);
            if(++timeout > 50) {
                portEXIT_CRITICAL(&mux); // Enables local interrupts
                return ESP_ERR_FLASH_BASE;
            }
        }

        uint8_t high_duration = 0;
        while(gpio_get_level(pin_num) != 0) {
            esp_rom_delay_us(1);
            if(++high_duration > 70) {
                portEXIT_CRITICAL(&mux); // Enables local interrupts
                return ESP_ERR_INVALID_STATE;
            }
        }

        // 3. Threshold using 28µs
        if (high_duration > 28) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }
    portEXIT_CRITICAL(&mux); // Enables local interrupts

    // Checksum for data
    if((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4]) {
        return ESP_ERR_NO_MEM;
    }

    // Combines integer and decimal point data into a 16 bit integer
    int16_t raw_humidity = (data[0] << 8) | data[1];
    int16_t raw_temperature = (data[2] << 8) | data[3];

    // Negative Temperature Handling
    if (raw_temperature & 0x8000) {
        raw_temperature &= 0x7FFF;
        *temp = ((float)raw_temperature / 10.0) * -1.0;
    } else {
        *temp = (float)raw_temperature / 10.0;
    }

    // Humidity
    *humid = (float)raw_humidity / 10.0;
    return ESP_OK;
}

static esp_err_t await_DHT22_response(float *temp, float *humid) {
    // Timeout if DHT22 doesn't response after 80µs
    timeout = 0;
    while(gpio_get_level(pin_num) == 0) {
        esp_rom_delay_us(1);
        if(++timeout > 80) {
            return ESP_ERR_TIMEOUT;
        }
    }

    // Timeout if DHT22 doesn't response after 80µs
    timeout = 0;
    while(gpio_get_level(pin_num) != 0) {
        esp_rom_delay_us(1);
        if(++timeout > 80) {
            return ESP_ERR_TIMEOUT;
        }
    }

    // Third Phase (Receive DHT22 Signal)
    return receive_DHT22_Signal(temp, humid);
}

esp_err_t dht22_start(float *temp, float *humid) {
    // First Phase (Send start signal for DHT22)
    gpio_set_level(pin_num, 1);
    gpio_set_level(pin_num, 0);
    vTaskDelay(pdMS_TO_TICKS(18)); // 18ms Delay
    gpio_set_level(pin_num, 1);
    esp_rom_delay_us(40); // 40µs wait for DHT22 to response a start signal

    // Delay between DHT22 Reading signal, and pulling from HIGH to LOW takes time, which the Timeout is keeping it safe.
    timeout = 0;
    while(gpio_get_level(pin_num) != 0) {
        esp_rom_delay_us(1);
        if(++timeout > 50) { // !IMPORTANT!
            return ESP_ERR_TIMEOUT;
        }
    }

    // Second Phase (DHT22 Response)
    return await_DHT22_response(temp, humid);
}

void dhtPin_Setup(uint64_t pin_mask) {
    gpio_init(&DHT22_Pin, pin_mask, GPIO_MODE_INPUT_OUTPUT_OD, GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_INTR_DISABLE);
    pin_num = __builtin_ffsll(pin_mask) - 1; // Convert bit_mask to GPIO_NUM
}