#pragma once
#include "i_led_controller.h"
#include <driver/gpio.h>
#include <Arduino.h>

/**
 * @class LEDController
 * @brief ESP32-specific implementation of ILEDController interface
 * 
 * Concrete implementation that controls an LED connected to a GPIO pin
 * on the ESP32 microcontroller.
 */
class LEDController : public ILEDController {
public:
    /**
     * @brief Constructs an LEDController for a specific GPIO pin
     * 
     * @param pin GPIO pin number to which the LED is connected
     */
    LEDController(uint8_t pin);

    /**
     * @brief Initializes the GPIO pin and sets initial LED state
     * 
     * @param initialState Initial state (0 = off, non-zero = on)
     */
    virtual void setup(uint32_t initialState = 0) override;
    
    /**
     * @brief Sets the LED state
     * 
     * @param state true to turn on, false to turn off
     */
    virtual void setState(bool state) override;
    
    /**
     * @brief Toggles the LED state
     * 
     * Inverts the current LED state
     */
    virtual void toggle() override;

private:
    uint8_t pin;           /**< GPIO pin number for the LED */
    bool currentState = false; /**< Current LED state (on/off) */
};
