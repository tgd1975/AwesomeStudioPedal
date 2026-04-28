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
        led.setState(true);
        delay(blinkDuration);
        led.setState(false);
        if (i < blinkCount - 1)
        {
            delay(blinkDuration);
        }
    }
}
