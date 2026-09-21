#include <unity.h>
#include "myBUZZER_Setup.hpp"
#include "myESP_SSD1306_I2C_Setup.h"

void setUp(void) {}
void tearDown(void) {}


// -------------------- Temperature Test START --------------------
// Below lower threshold (e.g., 15.0°C)
void temp_below_lower_threshold(void) {
    TEST_ASSERT_EQUAL(AlarmState::LOW_TEMPERATURE, evaluateTemperature(15.0f));
}

// Exactly lower threshold (18.0°C)
void temp_exactly_lower_threshold(void) {
    // Your code: if(temperature <= 18.0f) returns LOW_TEMPERATURE
    TEST_ASSERT_EQUAL(AlarmState::LOW_TEMPERATURE, evaluateTemperature(18.0f));
}

// Normal value (e.g., 24.0°C)
void temp_normal_value(void) {
    TEST_ASSERT_EQUAL(AlarmState::NORMAL, evaluateTemperature(24.0f));
}

// Exactly upper threshold (30.0°C)
void temp_exactly_upper_threshold(void) {
    // Your code: if(temperature >= 30.0f) returns HIGH_TEMPERATURE
    TEST_ASSERT_EQUAL(AlarmState::HIGH_TEMPERATURE, evaluateTemperature(30.0f));
}

// Above upper threshold (e.g., 35.0°C)
void temp_above_upper_threshold(void) {
    TEST_ASSERT_EQUAL(AlarmState::HIGH_TEMPERATURE, evaluateTemperature(35.0f));
}
// -------------------- Temperature Test END --------------------



// -------------------- Display Test Clockwise START --------------------
// Display Starting Point
void display_Initial_right(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::TEMPERATURE, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("20.0 C", text);
}

// Display Rotate Clockwise Once
void display_clockwise1(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::HUMIDITY, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("75.0 RH", text);
}

// Display Rotate Clockwise Twice
void display_clockwise2(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::LIGHT, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("500", text);
}

// Display Clockwise Thrice
void display_clockwise3(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::MOTION, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("DETECTED", text);
}

// Display Clockwise Fourth
void display_clockwise4(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::TEMPERATURE, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("20.0 C", text);
}
// -------------------- Display Test Clockwise END --------------------



// -------------------- Display Test Counter-Clockwise START --------------------
// Display Starting Point
void display_Initial_left(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::TEMPERATURE, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("20.0 C", text);
}

void display_counter_clockwise1(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::MOTION, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("DETECTED", text);
}

void display_counter_clockwise2(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::LIGHT, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("500", text);
}

void display_counter_clockwise3(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::HUMIDITY, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("75.0 RH", text);
}

void display_counter_clockwise4(void) {
    char text[16];
    unittest_display_update_screen((uint8_t)DisplayMode::TEMPERATURE, text, sizeof(text));
    TEST_ASSERT_EQUAL_STRING("20.0 C", text);
}
// -------------------- Display Test Counter-Clockwise END --------------------

extern "C" void app_main(void) {
    UNITY_BEGIN();

    // --- Temperature Test START ---
    RUN_TEST(temp_below_lower_threshold);
    RUN_TEST(temp_exactly_lower_threshold);
    RUN_TEST(temp_normal_value);
    RUN_TEST(temp_exactly_upper_threshold);
    RUN_TEST(temp_above_upper_threshold);
    // --- Temperature Test END ---

    // --- Display Test Clockwise START ---
    RUN_TEST(display_Initial_right);
    RUN_TEST(display_clockwise1);
    RUN_TEST(display_clockwise2);
    RUN_TEST(display_clockwise3);
    RUN_TEST(display_clockwise4);
    // --- Display Test Clockwise END ---

    // --- Display Test Counter-Clockwise START ---
    RUN_TEST(display_Initial_left);
    RUN_TEST(display_counter_clockwise1);
    RUN_TEST(display_counter_clockwise2);
    RUN_TEST(display_counter_clockwise3);
    RUN_TEST(display_counter_clockwise4);
    // --- Display Test Counter-Clockwise END ---

    UNITY_END();
}