/**
 * LED hardware diagnostic — platform-agnostic.
 *
 * Walks through every LED defined in hardwareConfig (power, bluetooth,
 * and each select LED) one at a time, lighting it for ~500 ms and
 * printing a line to serial identifying which LED and pin is active.
 * Loops forever; interrupt by power-cycling or reflashing production.
 *
 * Intent: isolate LED-hardware / GPIO-wiring problems from application
 * state (e.g. "blue LED doesn't light when connected" — is the LED
 * dead, the pin wrong, or the connection-state detection broken?).
 *
 * Compiled into two firmware variants:
 *   - env:nodemcu-32s-leds-test        (ESP32 NodeMCU-32S)
 *   - env:adafruit-feather-nrf52840-leds-test
 * Both pull the correct hardwareConfig via lib_extra_dirs.
 *
 * Flash with: make test-esp32-leds  (ESP32)
 *             make test-nrf52840-leds  (nRF52840)
 */

#include "config.h"
#include <Arduino.h>

static constexpr uint32_t STEP_MS = 500;
static constexpr uint8_t MAX_LEDS = 10;

static uint8_t allPins_[MAX_LEDS];
static char allLabels_[MAX_LEDS][16];
static uint8_t numPins_ = 0;

void setup()
{
    Serial.begin(115200);
    delay(2000);
    Serial.println("[LED_TEST] LED hardware diagnostic");

    // Build the ordered list of (pin, label) from hardwareConfig.
    allPins_[numPins_] = hardwareConfig.ledPower;
    strncpy(allLabels_[numPins_], "POWER", sizeof(allLabels_[0]));
    numPins_++;

    allPins_[numPins_] = hardwareConfig.ledBluetooth;
    strncpy(allLabels_[numPins_], "BLUETOOTH", sizeof(allLabels_[0]));
    numPins_++;

    for (uint8_t i = 0; i < hardwareConfig.numSelectLeds && numPins_ < MAX_LEDS; i++)
    {
        allPins_[numPins_] = hardwareConfig.ledSelect[i];
        snprintf(allLabels_[numPins_], sizeof(allLabels_[0]), "SELECT[%d]", i);
        numPins_++;
    }

    for (uint8_t i = 0; i < numPins_; i++)
    {
        pinMode(allPins_[i], OUTPUT);
        digitalWrite(allPins_[i], LOW);
        Serial.printf("  configured %s on pin %d\n", allLabels_[i], allPins_[i]);
    }

    Serial.printf(
        "[LED_TEST] READY — cycling %d LEDs every %lu ms\n", numPins_, (unsigned long) STEP_MS);
}

void loop()
{
    for (uint8_t i = 0; i < numPins_; i++)
    {
        for (uint8_t j = 0; j < numPins_; j++)
        {
            digitalWrite(allPins_[j], LOW);
        }
        digitalWrite(allPins_[i], HIGH);
        Serial.printf("LED %d: %s (pin %d)\n", i, allLabels_[i], allPins_[i]);
        delay(STEP_MS);
    }
}
