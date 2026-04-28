/**
 * On-device GPIO testrig for PinAction — NodeMCU-32S (GPIO 2, active-high onboard LED).
 *
 * Each PinAction mode is exercised against real GPIO hardware.  digitalRead(2) asserts
 * verify pin state after execute() and executeRelease().
 *
 * Porting note: if the target board has an active-low LED, PinLow / PinLowWhilePressed
 * become the visually confirming cases.  The digitalRead assertions remain correct
 * regardless of LED polarity.
 *
 * Requires: ESP32 (NodeMCU-32S) connected via USB.
 * Run with: make test-esp32-pin-io
 */
#include "pin_action.h"
#include <Arduino.h>
#include <unity.h>

static constexpr uint8_t TEST_PIN = 2; // NodeMCU-32S onboard LED, active-high

void setUp()
{
    pinMode(TEST_PIN, OUTPUT);
    digitalWrite(TEST_PIN, LOW);
}

void tearDown() {}

// ---------------------------------------------------------------------------

void test_PinHigh_execute_drives_high()
{
    PinAction action(Action::Type::PinHigh, TEST_PIN);
    action.execute();
    TEST_ASSERT_EQUAL(HIGH, digitalRead(TEST_PIN));
}

void test_PinLow_execute_drives_low()
{
    digitalWrite(TEST_PIN, HIGH); // pre-set high so change is visible
    PinAction action(Action::Type::PinLow, TEST_PIN);
    action.execute();
    TEST_ASSERT_EQUAL(LOW, digitalRead(TEST_PIN));
}

void test_PinToggle_first_execute_drives_high()
{
    PinAction action(Action::Type::PinToggle, TEST_PIN);
    action.execute();
    TEST_ASSERT_EQUAL(HIGH, digitalRead(TEST_PIN));
}

void test_PinToggle_second_execute_drives_low()
{
    PinAction action(Action::Type::PinToggle, TEST_PIN);
    action.execute(); // HIGH
    action.execute(); // LOW
    TEST_ASSERT_EQUAL(LOW, digitalRead(TEST_PIN));
}

void test_PinHighWhilePressed_execute_drives_high_release_drives_low()
{
    PinAction action(Action::Type::PinHighWhilePressed, TEST_PIN);
    action.execute();
    TEST_ASSERT_EQUAL(HIGH, digitalRead(TEST_PIN));
    action.executeRelease();
    TEST_ASSERT_EQUAL(LOW, digitalRead(TEST_PIN));
}

void test_PinLowWhilePressed_execute_drives_low_release_drives_high()
{
    digitalWrite(TEST_PIN, HIGH); // pre-set high
    PinAction action(Action::Type::PinLowWhilePressed, TEST_PIN);
    action.execute();
    TEST_ASSERT_EQUAL(LOW, digitalRead(TEST_PIN));
    action.executeRelease();
    TEST_ASSERT_EQUAL(HIGH, digitalRead(TEST_PIN));
}

// ---------------------------------------------------------------------------

void setup()
{
    delay(2000); // allow serial monitor to connect

    UNITY_BEGIN();
    RUN_TEST(test_PinHigh_execute_drives_high);
    RUN_TEST(test_PinLow_execute_drives_low);
    RUN_TEST(test_PinToggle_first_execute_drives_high);
    RUN_TEST(test_PinToggle_second_execute_drives_low);
    RUN_TEST(test_PinHighWhilePressed_execute_drives_high_release_drives_low);
    RUN_TEST(test_PinLowWhilePressed_execute_drives_low_release_drives_high);
    UNITY_END();
}

void loop() {}
