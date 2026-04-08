#pragma once
#include "i_button_controller.h"
#include <Arduino.h>
#include <driver/gpio.h>

/**
 * @class ButtonController
 * @brief ESP32-specific implementation of IButtonController interface
 *
 * Concrete implementation that reads button state from a GPIO pin
 * on the ESP32 microcontroller with pull-up resistor configuration.
 */
class ButtonController : public IButtonController
{
public:
    /**
     * @brief Constructs a ButtonController for a specific GPIO pin
     *
     * @param pin GPIO pin number to which the button is connected
     */
    ButtonController(uint8_t pin);

    /**
     * @brief Initializes the GPIO pin for button input
     *
     * Configures the pin as input with pull-up resistor.
     */
    virtual void setup() override;

    /**
     * @brief Reads the current button state
     *
     * @return true if button is pressed (active low), false otherwise
     */
    virtual bool read() override;

private:
    uint8_t pin; /**< GPIO pin number for the button */
};
