#include "led_controller.h"

/**
 * @brief Constructs an LEDController for a specific GPIO pin
 *
 * @param pin GPIO pin number to which the LED is connected
 */
LEDController::LEDController(uint8_t pin) : pin(pin) {}

/**
 * @brief Initializes the GPIO pin and sets initial LED state
 *
 * Configures the GPIO pin as an output and sets the initial state.
 *
 * @param initialState Initial state (0 = off, non-zero = on)
 */
void LEDController::setup(uint32_t initialState)
{
    gpio_pad_select_gpio(static_cast<gpio_num_t>(pin));
    gpio_set_direction(static_cast<gpio_num_t>(pin), GPIO_MODE_OUTPUT);
    setState(initialState);
}

/**
 * @brief Sets the LED state
 *
 * Only updates the GPIO pin if the state actually changes.
 *
 * @param state true to turn on, false to turn off
 */
void LEDController::setState(bool state)
{
    if (currentState != state)
    {
        gpio_set_level(static_cast<gpio_num_t>(pin), state ? 1 : 0);
        currentState = state;
    }
}

/**
 * @brief Toggles the LED state
 *
 * Inverts the current LED state by calling setState with the opposite value.
 */
void LEDController::toggle() { setState(! currentState); }
