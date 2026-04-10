#ifdef HOST_TEST_BUILD
#include "arduino_shim.h"

namespace fake_time
{
    unsigned long value = 0;
}

namespace fake_gpio
{
    int pin_state = LOW;
}

FakeSerial Serial;
#endif // HOST_TEST_BUILD
