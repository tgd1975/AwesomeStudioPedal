#ifdef HOST_TEST_BUILD
#  include "arduino_shim.h"
#else
// On device, pull in the real Arduino framework header.
// This file exists only so that -Itest/fakes doesn't shadow the framework Arduino.h.
#  include_next <Arduino.h>
#endif
