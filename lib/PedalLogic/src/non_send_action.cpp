#include "non_send_action.h"
#include <Arduino.h>

LEDBlinkAction::LEDBlinkAction(ILEDController& led, uint32_t blinkCount, uint32_t blinkDuration)
    : led(led), blinkCount(blinkCount), blinkDuration(blinkDuration)
{
}

void LEDBlinkAction::execute()
{
    for (uint32_t i = 0; i < blinkCount; i++)
    {
        led.setState(true);  // Turn LED on
        delay(blinkDuration);
        led.setState(false); // Turn LED off
        if (i < blinkCount - 1) // Don't delay after last blink
        {
            delay(blinkDuration);
        }
    }
}