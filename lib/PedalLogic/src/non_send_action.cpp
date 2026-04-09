#include "non_send_action.h"
#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#endif

LEDBlinkAction::LEDBlinkAction(ILEDController& led, uint32_t blinkCount, uint32_t blinkDuration)
    : led(led), blinkCount(blinkCount), blinkDuration(blinkDuration)
{
}

void LEDBlinkAction::execute()
{
    for (uint32_t i = 0; i < blinkCount; i++)
    {
        led.setState(true); // Turn LED on
#ifndef HOST_TEST_BUILD
        delay(blinkDuration);
#else
        // In host tests, we can't use delay, so just skip it
        (void) blinkDuration; // Silence unused variable warning
#endif
        led.setState(false);    // Turn LED off
        if (i < blinkCount - 1) // Don't delay after last blink
        {
#ifndef HOST_TEST_BUILD
            delay(blinkDuration);
#else
            (void) blinkDuration; // Silence unused variable warning
#endif
        }
    }
}