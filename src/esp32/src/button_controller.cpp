#include "button_controller.h"

/**
 * @brief Constructs a ButtonController for a specific GPIO pin
 *
 * @param pin GPIO pin number to which the button is connected
 */
ButtonController::ButtonController(uint8_t pin) : pin(pin) {}

/**
 * @brief Initializes the GPIO pin for button input
 *
 * Configures the pin as input with pull-up resistor.
 */
void ButtonController::setup()
{
    gpio_pad_select_gpio(static_cast<gpio_num_t>(pin));
    pinMode(pin, INPUT_PULLUP);
}

/**
 * @brief Reads the current button state
 *
 * @return true if button is pressed (active low), false otherwise
 */
bool ButtonController::read()
{
    return digitalRead(pin) == LOW; // Active low due to pull-up
}
