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

## BLE integration tests and pairing_pin

Two complementary on-device tests cover the BLE surface:

- **`make test-esp32-ble-config`** — uploads/downloads profiles via the BLE Config service.
  Connects from `bleak`, no pairing involved (test fixture has `pairing_pin: null`).
- **`make test-esp32-ble-pairing`** — Linux-only smoke test for the pairing-PIN flow.
  Drives `bluetoothctl` via `pexpect` to pair with the right passkey, reconnect (bond reuse),
  and reject the wrong passkey. Replaces the old "open the Flutter app and type the PIN"
  manual procedure. Cross-platform coverage (Windows / macOS) is tracked separately.

### test-esp32-ble-config

The Makefile target uploads `test/test_ble_config_esp32/data/` (which has
`"pairing_pin": null`) by setting `PLATFORMIO_DATA_DIR` for the `uploadfs` step:

```make
PLATFORMIO_DATA_DIR=test/test_ble_config_esp32/data \
    pio run -e nodemcu-32s-ble-config-test --target uploadfs
```

`test_main.cpp` checks `hardwareConfig.pairingEnabled` at boot and halts with
`[BLE_TEST] ERROR: pairing_pin must be null for integration tests` if the wrong config
was flashed. The runner treats this as a hard failure with a clear message.

If you add a new BLE integration test that connects without pairing, ensure it uses the
test fixture config or explicitly sets `pairing_pin: null` in its data directory.

> **Hitting `Message recipient disconnected from message bus` or
> `BleakGATTProtocolError: UNLIKELY_ERROR` on Ubuntu with BlueZ 5.83?** That's a
> known upstream regression, not your build — see
> [KNOWN_ISSUES.md](KNOWN_ISSUES.md#ble-config-integration-test-disconnects-on-linux-with-bluez-583).

### test-esp32-ble-pairing

Pre-flight: requires `bluetoothctl` (BlueZ) and the `pexpect` Python package. The runner
will reflash production firmware + production filesystem (with `pairing_pin: 12345`),
erase the pedal's NVS bond store and any host bond, then exercise three scenarios:

1. Pair with the correct passkey (`012345`) → expect `Pairing successful`, `Paired+Bonded`.
2. Disconnect + reconnect → expect *no* second passkey prompt (bond reused).
3. Pair with a wrong passkey → expect `Failed to pair`, no bond created.

The runner cleans up its own bonds at the end so the host is left in a known state.

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
