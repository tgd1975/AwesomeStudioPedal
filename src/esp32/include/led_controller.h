#pragma once
#include "i_led_controller.h"
#include <Arduino.h>
#if __has_include(<driver/gpio.h>)
#include <driver/gpio.h>
#endif

/**
 * @class LEDController
 * @brief ESP32-specific implementation of ILEDController
 *
 * Supports immediate setState/toggle and non-blocking blink sequences
 * driven by update() calls from the main loop.
 */
class LEDController : public ILEDController
{
public:
    explicit LEDController(uint8_t pin);

    void setup(uint32_t initialState = 0) override;
    void setState(bool state) override;
    void toggle() override;

    void startBlink(uint32_t intervalMs, int16_t count = -1) override;
    void stopBlink() override;
    void update(uint32_t now) override;
    bool isBlinking() const override { return blinking; }

private:
    uint8_t pin;
    bool currentState = false;
    bool blinking = false;
    bool stateBeforeBlink = false;
    uint32_t blinkInterval = 0;
    int16_t blinkRemaining = 0; // -1 = infinite, 0 = done, >0 = half-cycles left
    uint32_t lastToggleTime = 0;
};
