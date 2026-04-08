#include "button.h"
#include <Arduino.h>

/**
 * @brief Constructs a Button object
 * 
 * @param PIN GPIO pin number for the button connection
 */
Button::Button(uint8_t PIN) {
    this->PIN = PIN;
}

/**
 * @brief Checks if debounce period has elapsed
 * 
 * Compares current time with last debounce timestamp to determine
 * if a new button press should be registered.
 * 
 * @return true if debounce period has elapsed, false otherwise
 */
bool Button::isDebounced() {
    unsigned long now = millis();
    if ((now - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = now;
        return true;
    }
    return false;
}

/**
 * @brief Interrupt service routine for button press detection
 * 
 * Called by hardware interrupt when button state changes.
 * Uses debouncing to filter out electrical noise.
 */
void Button::isr() {
    if (isDebounced()) {
        pressed = true;
    }
}

/**
 * @brief Checks for button press events and clears the flag
 * 
 * @return true if button was pressed since last call, false otherwise
 */
bool Button::event() {
    if (pressed) {
        pressed = false;
        return true;
    }
    return false;
}
