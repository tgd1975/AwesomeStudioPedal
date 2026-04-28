---
id: TASK-237
title: BLE pairing PIN — configurable in hardware config, applied to NimBLE security
status: closed
opened: 2026-04-23
closed: 2026-04-24
effort: Large (8-24h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
prerequisites: [TASK-236]
---

## Description

BLE pairing is currently unenforced — the config service characteristics have no
encryption requirement. This task introduces a configurable passkey stored in
`hardware_config.json` under `"pairing_pin"`.

The passkey is a 6-digit BLE passkey (valid range: 0–999999; displayed with leading zeros,
e.g. `12345` → `012345`). Note: 4-digit PINs are Bluetooth Classic — BLE uses 6-digit
passkeys. The default value is `12345`, with a Spaceballs reference hidden in the C++
constant definition.

**Null / absent means no security.** If `pairing_pin` is `null`, `""`, or absent from the
hardware config, no passkey and no encryption are applied — any BLE client can connect and
write without pairing. This is the intended mode for development hardware and the
integration test fixture, eliminating the need for the test runner to handle a pairing
ceremony programmatically. The test hardware config (`data/hardware_config_test.json` or
equivalent) should set `"pairing_pin": null` explicitly.

The PIN is applied via `NimBLEDevice::setSecurityPasskey()` and passkey-entry auth mode.
All affected layers — firmware, configurator, schema, and tests — are updated.

## Acceptance Criteria

- [ ] `hardware_config.json` schema includes optional `pairing_pin` field (integer
      0–999999, or `null`/absent for no security); the C++ default constant carries a
      Spaceballs comment
- [ ] Firmware reads `pairing_pin` from the loaded hardware config; if non-null, calls
      `NimBLEDevice::setSecurityPasskey(pin)` and enables passkey-entry auth so a BLE
      client must complete the pairing ceremony before writing to config characteristics;
      if null/absent, no security is applied (open access)
- [ ] Flutter configurator exposes the `pairing_pin` field in the hardware config editor
      (integer input + a "disable pairing" toggle or empty-to-disable UX)
- [ ] A canonical test hardware config file (e.g. `data/hardware_config_test.json`) always
      has `"pairing_pin": null`; the `nodemcu-32s-ble-config-test` PlatformIO env flashes
      this file so future test authors never need to think about it
- [ ] The test firmware (`test_main.cpp`) asserts `pairing_pin == null` at startup and
      prints `[BLE_TEST] ERROR: pairing_pin must be null for integration tests` then halts
      if a non-null PIN is detected; `runner.py` treats this as a hard infrastructure
      failure with a clear, actionable error message
- [ ] `TESTING.md` documents the requirement: any BLE integration test that connects
      without pairing must use a hardware config with `pairing_pin: null`, and explains
      why (programmatic pairing on Linux via bleak/BlueZ is not feasible)
- [ ] Host tests cover `pairing_pin` parsing: present with valid value, `null`, absent
      (defaults to no-security), and out-of-range value (validation error)

## Test Plan

**Host tests** (`make test-host`):

- Extend hardware config parsing tests (`test/unit/test_hardware_config.cpp`) for
  `pairing_pin`: valid value, `null`, absent, out-of-range
- Register in `test/CMakeLists.txt` if a new file is added

**On-device tests** (`make test-esp32-ble-config`):

- Canonical test hardware config (`pairing_pin: null`) is flashed automatically by the
  test env → runner connects without pairing, all 7 scenarios pass
- Verify the startup guard: flash with a non-null PIN in the test config, confirm
  `[BLE_TEST] ERROR` is printed and runner exits with a clear failure message
- Manual smoke test with a non-null PIN: pair from phone (Flutter app or nRF Connect),
  confirm write is rejected before pairing and accepted after
- Requires: ESP32 connected via USB, BLE-capable host

## Prerequisites

- **TASK-236** — updates `runner.py` to use name-prefix discovery and connect to
  production firmware; required before the PIN feature can be tested end-to-end via the
  integration test runner

## Notes

- `null` / absent `pairing_pin` fully replaces the runtime role of `BLE_CONFIG_TEST_BUILD`
  (`setSecurityAuth(false,false,false)`). TASK-236 retires the compile-time guard;
  this task provides the clean runtime equivalent for test configs.
- Spaceballs reference placement suggestion: the C++ default constant —
  `// "That's the stupidest combination I've ever heard in my life!" — Spaceballs (1987)`
- BLE passkey range is 0–999999 (6 digits); `12345` is valid and renders as `012345`.
  4-digit PINs are Bluetooth Classic, not applicable here.
- Linux `runner.py` pairing via bleak/BlueZ is known-hard (no interactive agent);
  the `null`-pin approach sidesteps this entirely for the automated test. Full pairing
  coverage is handled by manual testing with the Flutter app.
- The three-layer guardrail for future test authors:
  1. **Canonical config** — test env always flashes `pairing_pin: null`; no manual step needed
  2. **Firmware startup assertion** — loud serial error + halt if PIN is non-null at test time
  3. **`TESTING.md` docs** — explains the constraint and the reason so the next person
     understands rather than just cargo-culting `null`
