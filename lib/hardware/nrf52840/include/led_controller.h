#pragma once
#include "i_led_controller.h"
#include <Arduino.h>

/**
 * @class LEDController
 * @brief nRF52840-specific implementation of ILEDController interface
 *
 * Controls an LED on a GPIO pin using the Arduino digitalWrite API.
 */
class LEDController : public ILEDController
{
public:
    /**
     * @brief Constructs an LEDController for a specific pin
     * @param pin GPIO pin number to which the LED is connected
     */
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
    int16_t blinkRemaining = 0;
    uint32_t lastToggleTime = 0;
};
