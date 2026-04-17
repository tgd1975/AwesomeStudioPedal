#ifdef HOST_TEST_BUILD
#include "arduino_shim.h"

namespace fake_time
{
    unsigned long value = 0;
}

namespace fake_gpio
{
    int pin_state = LOW;
    int written_pin = -1;
    int written_value = -1;
}

FakeSerial Serial;
#endif // HOST_TEST_BUILD
