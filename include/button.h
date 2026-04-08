#pragma once
#include <Arduino.h>

/**
 * @class Button
 * @brief Handles button input with debouncing for ESP32
 * 
 * This class manages button press detection with hardware debouncing
 * to prevent false triggers from electrical noise.
 */
class Button {
  private:
    uint8_t PIN; /**< GPIO pin number for the button */
    
    /**
     * @brief Checks if enough time has passed since last debounce
     * @return true if debounce period has elapsed, false otherwise
     */
    bool isDebounced();

  public:
    volatile bool pressed = false; /**< Flag indicating button was pressed */
    unsigned long lastDebounceTime = 0;  /**< Timestamp of last debounce event */
    unsigned long debounceDelay = 300;    /**< Debounce delay in milliseconds */

    /**
     * @brief Constructs a Button object
     * @param PIN GPIO pin number to which the button is connected
     */
    Button(uint8_t PIN);

    /**
     * @brief Interrupt service routine - called when button state changes
     * 
     * Checks debounce timing and sets pressed flag if valid press detected
     */
    void isr();
    
    /**
     * @brief Checks if button was pressed and clears the flag
     * @return true if button was pressed since last call, false otherwise
     */
    bool event();
};
