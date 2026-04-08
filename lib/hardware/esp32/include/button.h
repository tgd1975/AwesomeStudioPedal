#pragma once
#include "i_button.h"
#include <Arduino.h>

/**
 * @class Button
 * @brief ESP32 interrupt-driven button with debouncing
 *
 * Implements IButton using hardware GPIO interrupts and millis()-based
 * debouncing. isr() must be called from an IRAM_ATTR interrupt handler
 * attached with attachInterrupt().
 */
class Button : public IButton {
  private:
    uint8_t PIN; /**< GPIO pin number */

    /**
     * @brief Returns true if the debounce period has elapsed since the last press
     */
    bool isDebounced();

  public:
    volatile uint8_t pressCount = 0;     /**< Incremented by ISR, decremented by event() */
    volatile bool awaitingRelease = false; /**< True after press, until pin goes HIGH */
    unsigned long lastDebounceTime = 0;  /**< Timestamp of last accepted press */
    unsigned long debounceDelay = 500;   /**< Debounce window in milliseconds */

    /**
     * @brief Constructs a Button for the given GPIO pin
     * @param PIN GPIO pin number
     */
    explicit Button(uint8_t PIN);

    /**
     * @brief Configures the GPIO pin as input with pull-up
     *
     * Must be called before attachInterrupt().
     */
    void setup() override;

    /**
     * @brief ISR entry point — call from an IRAM_ATTR interrupt handler
     *
     * Applies debouncing and sets the pressed flag if a valid press is detected.
     */
    void isr();

    /**
     * @brief Checks for a press event and clears the flag
     * @return true if pressed since last call, false otherwise
     */
    bool event() override;

    /**
     * @brief Clears the pressed flag
     *
     * Call before attachInterrupt() to discard any stale press.
     */
    void reset() override;
};
