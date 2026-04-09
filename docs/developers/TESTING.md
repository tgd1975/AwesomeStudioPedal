# Testing

## Two test worlds

This is the most important concept in the test setup.

**Host tests** (GoogleTest + CMake) run on the development machine. They test pure logic using mock
and fake hardware objects. No physical device is needed, and they run in milliseconds.

**On-device tests** (Unity + PlatformIO) run on a physical ESP32. They test actual hardware
interaction — button debounce, GPIO state, serial output. A connected device is required.

Use host tests for logic, on-device tests for hardware behaviour.

## The hardware seam

The codebase uses a hardware abstraction seam to make host testing possible:

- The `HOST_TEST_BUILD` preprocessor flag strips out Arduino/ESP-IDF headers when building for the
  host.
- `test/fakes/arduino_shim.h` provides stub implementations of Arduino types and functions used
  by the logic layer.
- Three interface classes — `ILEDController`, `IButtonController`, `IBleKeyboard` — define the
  contract between platform-independent logic and hardware drivers.

Mock implementations of these interfaces live in `test/mocks/`. The host test build links against
the mocks instead of the real hardware drivers.

## Running tests

| Command | Framework | Requires hardware |
|---------|-----------|------------------|
| `make test-host` | GoogleTest | No |
| `make test-esp32-button` | Unity | Yes (ESP32) |
| `make test-esp32-serial` | Unity | Yes (ESP32) |
| `make test-nrf52840-button` | Unity | Yes (nRF52840) |

## Writing a new host test

1. Create a file in `test/unit/`, e.g. `test_my_feature.cpp`.
2. Include GoogleTest: `#include <gtest/gtest.h>`
3. Use mock classes from `test/mocks/` as needed.
4. Register the test suite in `CMakeLists.txt` under the `pedal_tests` target.

Example:

```cpp
#include <gtest/gtest.h>
#include "mocks/mock_ble_keyboard.h"
#include "profile_manager.h"

TEST(ProfileManagerTest, SwitchProfileUpdatesIndex) {
    MockLEDController led1, led2, led3;
    ProfileManager pm(led1, led2, led3);
    pm.switchProfile();
    EXPECT_EQ(pm.getCurrentProfile(), 1);
}
```

## Mock classes

Location: `test/mocks/`

| Mock | Interface | What it stubs |
|------|-----------|---------------|
| `MockButtonController` | `IButtonController` | Button read state |
| `MockLEDController` | `ILEDController` | LED set/toggle |
| `MockBleKeyboard` | `IBleKeyboard` | Key press/release, connection state |

## Known startup noise

When running on-device tests you will see:

```text
E (156) esp_core_dump_flash: No core dump partition found!
```

This is harmless. The ESP32 checks for a core dump partition on every boot. Because
`config/esp32/partitions.csv` does not define a coredump partition, these lines appear before the
Unity test runner starts and have no effect on test results.

## Current coverage

| Component | Coverage estimate |
|-----------|-----------------|
| ButtonController | ~60% |
| LEDController | ~50% |
| ProfileManager | ~40% |
| EventDispatcher | 0% |
| Send hierarchy | 0% |
