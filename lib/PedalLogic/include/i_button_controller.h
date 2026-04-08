#pragma once

/**
 * @class IButtonController
 * @brief Interface for button input functionality
 * 
 * Abstract interface that defines the contract for button operations.
 * Allows for hardware abstraction and dependency injection.
 */
class IButtonController {
public:
    virtual ~IButtonController() = default;
    
    /**
     * @brief Initializes the button hardware
     * 
     * Configures GPIO pins and any necessary hardware resources.
     */
    virtual void setup() = 0;
    
    /**
     * @brief Reads the current button state
     * 
     * @return true if button is pressed, false otherwise
     */
    virtual bool read() = 0;
};
