#pragma once
#include <cstdint>

/**
 * @class ILEDController
 * @brief Interface for LED control functionality
 * 
 * Abstract interface that defines the contract for LED operations.
 * Allows for hardware abstraction and dependency injection.
 */
class ILEDController {
public:
    virtual ~ILEDController() = default;
    
    /**
     * @brief Initializes the LED with optional initial state
     * 
     * @param initialState Initial state (0 = off, non-zero = on)
     */
    virtual void setup(uint32_t initialState = 0) = 0;
    
    /**
     * @brief Sets the LED state
     * 
     * @param state true to turn on, false to turn off
     */
    virtual void setState(bool state) = 0;
    
    /**
     * @brief Toggles the LED state
     * 
     * If LED is on, turns it off. If off, turns it on.
     */
    virtual void toggle() = 0;
};
