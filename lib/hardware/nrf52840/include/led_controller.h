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

    /**
     * @brief Initializes the pin as output and sets initial state
     * @param initialState Initial state (0 = off, non-zero = on)
     */
    void setup(uint32_t initialState = 0) override;

    /**
     * @brief Sets the LED state
     * @param state true to turn on, false to turn off
     */
    void setState(bool state) override;

    /**
     * @brief Toggles the LED state
     */
    void toggle() override;

private:
    uint8_t pin;               /**< GPIO pin number */
    bool currentState = false; /**< Current LED state */
};
