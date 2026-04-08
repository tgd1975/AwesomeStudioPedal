#pragma once
#include "i_button.h"
#include <cstdint>

/**
 * @class Button
 * @brief nRF52840-specific implementation of IButton interface
 *
 * Interrupt-driven button with software debouncing. Attach isr() to a
 * falling-edge interrupt on the button pin.
 */
class Button : public IButton {
private:
    uint8_t PIN;
    bool isDebounced();

public:
    volatile bool pressed = false;      /**< Set by ISR; cleared by event() */
    unsigned long lastDebounceTime = 0; /**< Timestamp of last accepted ISR */
    unsigned long debounceDelay = 300;  /**< Minimum ms between accepted events */

    /**
     * @brief Constructs a Button for the given pin
     * @param PIN GPIO pin number
     */
    explicit Button(uint8_t PIN);

    /** @brief Configures the pin as input with pull-up */
    void setup() override;

    /**
     * @brief Interrupt service routine — call from a falling-edge ISR
     *
     * Sets the pressed flag after debounce check.
     */
    void isr();

    /**
     * @brief Consumes and returns a pending press event
     * @return true if a press event was pending
     */
    bool event() override;

    /** @brief Clears any pending press event */
    void reset() override;
};
