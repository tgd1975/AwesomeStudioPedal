# BLE Config Service — Implementation Notes

This document records the engineering challenges, dead ends, and architectural decisions
made during the implementation of the BLE Config service on ESP32 (NimBLE stack). It is
intended for future maintainers and anyone debugging BLE connectivity on Linux/BlueZ hosts.

---

## Stack choice: NimBLE vs classic ESP32 BLE

The project uses the `ESP32-BLE-Keyboard` library, which supports both the classic
Arduino ESP32 BLE stack and NimBLE. NimBLE is the preferred stack (smaller RAM footprint,
more actively maintained) and is enabled project-wide via `-DUSE_NIMBLE`.

**Critical constraint**: when `USE_NIMBLE` is defined, `ble_gatts_start()` is called
exactly once inside `BleKeyboard::begin()` — it atomically locks in every registered
GATT service. Any service added *after* this point is silently ignored. This meant the
Config GATT service had to be registered before advertising started.

---

## Challenge 1: Registering GATT services at the right moment

**Problem**: `BleConfigService::begin()` was originally called after `bleKeyboardAdapter->begin()`.
By then `ble_gatts_start()` had already run and the GATT table was locked. The config
service's characteristics were never actually registered, so the device advertised nothing
useful and BLE scans found no matching UUID.

**Solution**: `BleKeyboard` (in the fork used here) has a virtual `onStarted(BLEServer*)`
method that fires *inside* `begin()`, after HID services start but before `adv->start()`.
`HookableBleKeyboard` subclasses `BleKeyboard` and exposes this hook via
`setOnStartedCallback`. `BleConfigService::begin()` installs its GATT setup code as that
callback, so registration happens in the correct window.

**Ordering rule**: `bleConfigService.begin()` must always be called **before**
`bleKeyboardAdapter->begin()`.

---

## Challenge 2: Multiple-inheritance return-type conflict

**Problem**: An early design had `BleKeyboardAdapter` inherit from both `BleKeyboard` and
`IBleKeyboard`. `BleKeyboard` inherits `Print`, so its `write(uint8_t)` returns `size_t`.
`IBleKeyboard::write(uint8_t)` returns `void`. C++ does not allow an override to change
the return type — the compiler rejected this with an error about conflicting return types.

**Solution**: Switched to composition. `HookableBleKeyboard` subclasses only `BleKeyboard`
and adds the hook. `BleKeyboardAdapter` subclasses only `IBleKeyboard` and holds a reference
to a `HookableBleKeyboard` instance, delegating all calls. No multiple inheritance.

---

## Challenge 3: BlueZ HID daemon causing disconnects (historical)

**Problem**: On Linux, BlueZ runs a HID daemon (`bluetoothd` with the HID plugin) that
auto-connects to any device advertising the HID service UUID (`0x1812`). When
`BleKeyboard`'s advertisement included `0x1812`, BlueZ's HID daemon tried to read the
HID Report Map and other encrypted HID characteristics without pairing first. NimBLE
returned `Insufficient Authentication`. BlueZ then terminated the connection with
`BLE_ERR_REM_USER_CONN_TERM` (reason code `0x24`) before the BLE client could subscribe
to notifications or write any data.

**Root cause investigation**: The issue only manifested during automated integration tests
because the old test runner used UUID-only discovery (filtering on the Config service UUID).
Production firmware does not advertise the Config service UUID — it advertises HID UUID +
device name. The test runner therefore never found production firmware at all and a separate
HID-less test firmware was created to work around this. That created the disconnect problem.

**Resolution (TASK-236)**: The test runner now uses the same name-prefix discovery as the
CLI tool, and connects directly to production firmware. The CLI upload was tested on Linux
against production firmware and confirmed that the BlueZ HID daemon does not interfere in
practice. The separate HID-less test firmware and all associated guards were removed.

---

## Challenge 4: Write characteristics require WRITE | WRITE_NR

**Problem**: The characteristics were originally declared with only
`NIMBLE_PROPERTY::WRITE_NR` (write-without-response). BlueZ's D-Bus GATT backend uses
`AcquireWrite` for write-without-response and `WriteValue` for write-with-response.
`AcquireWrite` requires an established L2CAP channel and failed with
`org.bluez.Error.Failed: Failed to initiate write` until the ATT MTU negotiation completed.
The 0.5 s post-connect delay added to work around this was insufficient.

**Solution**: Declare both `NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::WRITE` on the
config write characteristics. This allows BlueZ to use the simpler `WriteValue` D-Bus
path (`response=True` in bleak), which is more robust during connection setup. The extra
ACK from write-with-response adds one round trip per chunk but is negligible given chunk
sizes are up to 510 bytes.

The protocol spec (`BLE_CONFIG_PROTOCOL.md`) retains `WRITE_NO_RESPONSE` as the
*canonical* property; the `WRITE` property is an implementation addition for BlueZ
compatibility.

---

## Challenge 5: BlueZ GATT cache stale entries

**Problem**: After changing the test firmware's GATT layout, BlueZ still had a cached GATT
table from previous connections. Writes to the config characteristic failed because BlueZ
was using a stale handle mapping.

**Solution**: `bluetoothctl remove <addr>` clears the cached device entry. This needs to
be done once when the firmware changes its GATT layout significantly. The runner does not
need to do this automatically — after the initial cache clear the new layout is cached
correctly and subsequent test runs work without intervention.

---

## Challenge 6: Missing READY line — serial timing

**Problem**: The Makefile runs `pio upload` then immediately launches `runner.py`. The
ESP32 boots in ~2 s and prints `[BLE_TEST] READY`, but the runner sometimes opened the
serial port after that line had already been sent. The runner then waited 30 s for a READY
line that never came, timing out and failing.

**Solution**: `runner.py` pulses DTR low → high after opening the serial port. This
triggers the ESP32 hardware reset via the USB-to-UART chip's DTR line, producing a fresh
boot with READY visible from the start of the serial stream.

---

## Challenge 7: Serial line race — PROFILE vs RESET

**Problem**: `loop()` prints `[BLE_TEST] PROFILE:<name>` whenever the active profile
index changes. During Test 4 (persistence), the test runner sent `RESET` and then read
the next `[BLE_TEST]` line looking for `[BLE_TEST] RESET`. A spontaneous
`[BLE_TEST] PROFILE:…` line arrived first, causing `read_serial_line` to return the wrong
line and the "soft-reset acknowledged" check to fail.

**Solution**: `read_serial_line` accepts an optional `keyword` parameter (default
`"[BLE_TEST]"`). Callers that expect a specific tag pass the exact string:

- `RESET` check: `keyword="[BLE_TEST] RESET"`
- `PROFILE?` response: `keyword="[BLE_TEST] PROFILE:"`

---

## Challenge 8: Active profile not reported after upload

**Problem**: The profile change fires on the very first `loop()` tick — before any BLE
connection is established. By the time the test runner finished uploading and started
reading serial, that line was long gone.

**Solution**: After a successful upload the runner sends a `PROFILE?` command and waits
for the `[BLE_TEST] PROFILE:` response. The firmware handles `PROFILE?` in `loop()` by
printing the current profile on demand, making the check deterministic.

---

## Challenge 9: Persistence across soft-reset — profiles not loaded on boot

**Problem**: After the soft-reset in Test 4, the firmware rebooted but `getProfile(0)`
returned `nullptr` — the profile manager was empty. The test harness did not call
`configureProfiles()`, so no profiles were loaded from LittleFS on boot.

**Solution**: `configureProfiles(*pm, nullptr)` is called in `setup()`. This reads
`profiles.json` from LittleFS (written there by `BleConfigReassembler` on the previous
upload). Profile index persistence across reboot uses `Preferences` (NVS).

---

## Security model

BLE pairing security is controlled by the `pairing_pin` field in the hardware config
(`/config.json` on LittleFS):

- **`pairing_pin` absent or `null`**: no passkey set, no encryption required on config
  characteristics. Any BLE client can connect and write without pairing. Used for
  development hardware and integration test fixtures.
- **`pairing_pin: <number>`**: `NimBLEDevice::setSecurityPasskey(pin)` is called in
  `HookableBleKeyboard::onStarted()` and passkey-entry auth is enabled. A client must
  complete the pairing ceremony before writing config data.

The default `data/config.json` **does not** set `pairing_pin`, so the shipping
firmware uses Just Works pairing. This default was changed (TASK-250 follow-up):
the previous default of `12345` enabled DisplayOnly + MITM, but the pedal has no
display, so Android/iOS hosts had no usable way to acquire the passkey and the
in-app scan flow was unreachable. Builders who want passkey-entry auth can opt
in by adding `pairing_pin` — see [docs/builders/HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md#ble-pairing-optional).

The integration test fixture (`test/test_ble_config_esp32/data/config.json`)
also sets `"pairing_pin": null` so the automated runner can connect without
pairing. The test firmware asserts `pairingEnabled == false` at startup and
halts with a clear error if the wrong config was flashed.

---

## Architecture summary

```
Production path (main.cpp / test_main.cpp)
──────────────────────────────────────────
HookableBleKeyboard ──subclasses──▶ BleKeyboard (HID)
BleKeyboardAdapter  ──wraps──────▶ HookableBleKeyboard
BleConfigService::begin(IBleKeyboard*, …)
  └─ casts to BleKeyboardAdapter
  └─ installs onStarted callback
  └─ inside BleKeyboard::begin(): setupGattService(pServer)
  └─ pairingEnabled → setSecurityPasskey(pin) + passkey auth
  └─ pairingEnabled == false → open access (test configs)
```
