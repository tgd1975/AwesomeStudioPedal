#pragma once
#include "action.h"
#include "i_led_controller.h"

/**
 * @class LEDBlinkAction
 * @brief Example of a non-send action that blinks an LED
 * 
 * Demonstrates how to create actions that don't involve keyboard input.
 */
class LEDBlinkAction : public Action
{
private:
    ILEDController& led; /**< Reference to LED controller */
    uint32_t blinkCount; /**< Number of times to blink */
    uint32_t blinkDuration; /**< Duration of each blink in milliseconds */

public:
    /**
     * @brief Constructs an LEDBlinkAction
     * 
     * @param led Reference to LED controller
     * @param blinkCount Number of times to blink
     * @param blinkDuration Duration of each blink in milliseconds
     */
    LEDBlinkAction(ILEDController& led, uint32_t blinkCount = 3, uint32_t blinkDuration = 200);
    
    /**
     * @brief Executes the LED blink action
     * 
     * Blinks the LED the specified number of times.
     */
    void execute() override;
    
    /**
     * @brief Checks if this action is a send action
     * 
     * @return false since this is not a send action
     */
    bool isSendAction() const override { return false; }
};