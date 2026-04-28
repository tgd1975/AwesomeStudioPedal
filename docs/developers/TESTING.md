# Testing

## Three test layers

This is the most important concept in the test setup.

**Host unit tests** (GoogleTest + CMake) run on the development machine.
They test pure logic using mock and fake hardware objects. No physical
device is needed, and they run in milliseconds.

**Host integration tests** (also GoogleTest + CMake) construct
`HostPedalApp` with a `MockBleKeyboard` and exercise the full
`BlePedalApp::setup()` / `loop()` flow on host. Press buttons via
`fake_gpio::setPinState`, advance time via `fake_time::value`, assert on
recorded mock calls. See `test/unit/test_app_integration.cpp`.

**On-device tests** (Unity + PlatformIO) run on a physical ESP32 or
nRF52840. They test actual hardware interaction — button debounce, GPIO
state, serial output, BLE pairing. A connected device is required.

Use host tests for logic and end-to-end behaviour; on-device tests for
hardware-specific concerns (timing under real interrupts, BLE host
interop, persistent storage).

## The hardware seam

The codebase uses a hardware abstraction seam to make host testing
possible. Five interfaces define the contract between platform-
independent logic and hardware drivers:

- `ILEDController` — set LED state
- `IButtonController` — read button state
- `IBleKeyboard` — send key events
- `IFileSystem` — read/write files
- `ILogger` — diagnostic output

Host tests link against `HostPedalApp` plus the host-side platform
implementations under `src/host/` (`host_logger.cpp`,
`host_file_system.cpp`, `host_config.cpp`). There is no preprocessor
flag — the build system (`test/CMakeLists.txt`) selects which
implementations to compile and link, and the `Arduino.h` shim under
`test/fakes/` provides no-op stubs for any Arduino API the production
code touches.

Mock implementations of the interfaces live in `test/mocks/` and are
used to assert on behaviour (e.g. `MockBleKeyboard.write(0x50)` was
called once).

## Running tests

| Command | Framework | Requires hardware |
|---------|-----------|------------------|
| `make test-host` | GoogleTest | No |
| `make test-esp32-button` | Unity | Yes (ESP32) |
| `make test-esp32-serial` | Unity | Yes (ESP32) |
| `make test-nrf52840-button` | Unity | Yes (nRF52840) |

## Writing a new host unit test

1. Create a file in `test/unit/`, e.g. `test_my_feature.cpp`.
2. Include GoogleTest: `#include <gtest/gtest.h>`
3. Use mock classes from `test/mocks/` as needed.
4. Register the test source file in `test/CMakeLists.txt` under the
   `pedal_tests` target.

Example:

```cpp
#include <gtest/gtest.h>
#include "mock_led_controller.h"
#include "profile_manager.h"

TEST(ProfileManagerTest, SwitchProfileAdvancesIndex)
{
    std::vector<ILEDController*> leds = { /* ... */ };
    ProfileManager pm(leds);
    EXPECT_EQ(pm.switchProfile(), 1);
}
```

## Writing a host integration test

Integration tests run the real `BlePedalApp::setup()` / `loop()` on
host with mocks for the BLE keyboard and per-pin control over button
inputs. See `test/unit/test_app_integration.cpp` for the canonical
pattern.

Example skeleton:

```cpp
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "config.h"
#include "host_pedal_app.h"
#include "mock_ble_keyboard.h"
#include "arduino_shim.h" // fake_gpio, fake_time

class AppIntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        fake_gpio::reset();
        fake_time::value = 0;

        // Buttons are active-low; default to "released".
        for (uint8_t i = 0; i < hardwareConfig.numButtons; i++)
            fake_gpio::setPinState(hardwareConfig.buttonPins[i], HIGH);
    }
};

TEST_F(AppIntegrationTest, ButtonAPress_EmitsConfiguredAction)
{
    MockBleKeyboard mockKb;
    EXPECT_CALL(mockKb, begin()).Times(1);
    EXPECT_CALL(mockKb, isConnected()).Times(::testing::AnyNumber());
    EXPECT_CALL(mockKb, print(::testing::StrEq(" "))).Times(1);

    HostPedalApp app(&mockKb);
    app.setup();

    // Simulate press + release with debounce-friendly timing
    const uint8_t pin = hardwareConfig.buttonPins[0];
    fake_time::value += 200;
    fake_gpio::setPinState(pin, LOW);
    app.onActionButtonInterrupt(0);
    fake_time::value += 150;
    fake_gpio::setPinState(pin, HIGH);
    app.onActionButtonInterrupt(0);
    fake_time::value += 400;  // past double-press window

    app.loop();
}
```

Key points:

- Construct one `HostPedalApp` per `TEST_F` — it sets a global ISR
  pointer (`g_blePedalApp`) so two concurrent instances would clash.
- Reset `fake_gpio` and `fake_time::value` in `SetUp()`.
- Advance `fake_time::value` past `Button`'s 100 ms debounce window
  before the first edge, then past the 300 ms double-press window
  before calling `loop()` — otherwise `event()` won't fire.

## Mocks and fakes

| Location | What | Used by |
|---|---|---|
| `test/mocks/mock_ble_keyboard.h` | `MockBleKeyboard : public IBleKeyboard` (gMock) | Unit + integration tests |
| `test/mocks/mock_led_controller.h` | `MockLEDController : public ILEDController` (gMock) | Unit tests |
| `test/fakes/null_logger.h` | `NullLogger : public ILogger` — no-op | Tests that don't care about logging |
| `test/fakes/null_led_controller.h` | `NullLedController : public ILEDController` — no-op | Profile manager tests, etc. |
| `test/fakes/arduino_shim.h` | Arduino API shims: `delay`, `millis`, `Serial`, per-pin `digitalRead`/`digitalWrite` | Any test that touches Arduino code |
| `src/host/include/host_pedal_app.h` | `HostPedalApp : public BlePedalApp` — host pedal app for integration tests | Integration tests |
| `src/host/src/host_logger.cpp` | `HostLogger : public ILogger` — writes to `std::cout` with `DEBUG:` prefix | All host tests via `createLogger()` |
| `src/host/src/host_file_system.cpp` | `HostFileSystem : public IFileSystem` — uses `std::ifstream`/`ofstream` | Config-loader tests |
| `src/host/src/host_config.cpp` | Static `hardwareConfig` global with sensible defaults | Any test that depends on `hardwareConfig` |

## Per-pin GPIO state

`fake_gpio::setPinState(pin, HIGH|LOW)` sets one pin's read state.
`fake_gpio::getPinState(pin)` reads it. `fake_gpio::reset()` clears all
per-pin state plus the legacy `pin_state` / `last_written_*` globals.
Tests that need multi-button scenarios (e.g. long-press on A while B is
held) use `setPinState`. Tests that touch only one pin can still set
`fake_gpio::pin_state` — it acts as a fallback for pins not explicitly
set.

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
| `BlePedalApp::loop()` end-to-end | covered by `test_app_integration.cpp` (4 scenarios) |
