#pragma once

// Host-test-only shim: when -Itest/fakes is on the include path (set up in
// test/CMakeLists.txt), this file is the first match for <Arduino.h> and
// transparently routes to arduino_shim.h. PIO on-device builds never have
// -Itest/fakes on their include path, so the real framework Arduino.h is
// resolved as usual on those targets.
#include "arduino_shim.h"
