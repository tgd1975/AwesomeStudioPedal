#pragma once
#include "i_button_controller.h"
#include <Arduino.h>

/**
 * @class ButtonController
 * @brief nRF52840-specific implementation of IButtonController interface
 *
 * Reads button state from a GPIO pin with pull-up resistor configuration.
 */
class ButtonController : public IButtonController {
public:
    /**
     * @brief Constructs a ButtonController for a specific pin
     * @param pin GPIO pin number to which the button is connected
     */
    explicit ButtonController(uint8_t pin);

    /**
     * @brief Initializes the pin as input with pull-up
     */
    void setup() override;

    /**
     * @brief Reads the current button state
     * @return true if button is pressed (active low)
     */
    bool read() override;

private:
    uint8_t pin; /**< GPIO pin number */
};
