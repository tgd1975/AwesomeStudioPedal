---
id: TASK-236
title: Retire BLE_CONFIG_TEST_BUILD — update runner.py to test production firmware path
status: closed
opened: 2026-04-23
closed: 2026-04-24
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
---

## Description

The BLE config integration test uses a special test firmware that bypasses BleKeyboard/HID
entirely and disables BLE authentication (`setSecurityAuth(false,false,false)`) via a
`BLE_CONFIG_TEST_BUILD` preprocessor guard. This was originally introduced to avoid BlueZ
HID daemon interference and Python runner pairing limitations on Linux.

Live testing on 2026-04-23 confirmed that the CLI upload tool (`pedal_config.py`) connects
to production firmware on Linux without any BlueZ interference. The root cause of the
original problem was the test runner's UUID-only device discovery — production firmware
(BleKeyboard) does not advertise the Config service UUID, only the HID UUID plus device
name. This meant `runner.py` could never find production firmware at all, and the entire
HID-less test firmware path was built around that limitation. The longer connection setup
time in the runner also gave BlueZ a larger window to interfere, but in practice it does
not.

The production code path is sound and should be what the integration test exercises. The
`BLE_CONFIG_TEST_BUILD` machinery (guard, CI check, pre-commit hook, second `begin()`
overload, injectable `HookInstaller`) can be retired.

## Acceptance Criteria

- [ ] `runner.py` discovers the pedal using the same name-prefix fallback as the CLI
      (`pedal_config.py`) and connects to production firmware (BleKeyboard active) on Linux
      without BlueZ interference
- [ ] `BLE_CONFIG_TEST_BUILD` is removed from `ble_config_service.cpp`, `platformio.ini`,
      the pre-commit hook, `release.yml`, `RELEASE_CHECKLIST.md`, and
      `BLE_CONFIG_IMPLEMENTATION_NOTES.md`; the second `begin()` overload and the
      injectable `HookInstaller` are removed from `BleConfigService`
- [ ] `test_main.cpp` is updated to use the production `begin()` path (via
      `BleKeyboardAdapter` / `setOnStartedCallback`) so the test firmware exercises the
      same code path as production
- [ ] All integration test scenarios pass against production firmware (`make test-esp32-ble-config`)

## Test Plan

**On-device tests** (`make test-esp32-ble-config`):

- Flash production firmware path via `nodemcu-32s-ble-config-test` env (updated to use
  BleKeyboard / `setOnStartedCallback`, no `BLE_CONFIG_TEST_BUILD`)
- Run `runner.py` on Linux — confirm device is found via name-prefix fallback
- All existing scenarios must pass: valid upload, LED confirmation, error recovery,
  persistence across soft-reset
- Requires: ESP32 connected via USB, BLE-capable Linux host

## Notes

- The `BleKeyboardAdapter::setOnStartedCallback` production path is already implemented
  and tested — the test firmware just needs to call `begin(pm, &bleKeyboardAdapter, leds)`
  instead of the injectable-hook overload.
- The injectable `HookInstaller` overload and all `BLE_CONFIG_TEST_BUILD` guards can be
  deleted outright; no backwards-compatibility shim needed.
- On 2026-04-21 explicit guardrails were added specifically to prevent `BLE_CONFIG_TEST_BUILD`
  from ever reaching production (since it strips all BLE security/pairing). These must all
  be removed as part of this task — they are no longer needed and leaving them creates dead
  weight:
  - **pre-commit hook** (`scripts/pre-commit`): scans `platformio.ini` and fails if
    `BLE_CONFIG_TEST_BUILD` appears in any non-test env's build flags
  - **CI step** (`.github/workflows/release.yml`): fails the release build if
    `BLE_CONFIG_TEST_BUILD` is present in production build flags
  - **`RELEASE_CHECKLIST.md`**: mandatory pre-release check for the flag
  - **`BLE_CONFIG_IMPLEMENTATION_NOTES.md`**: critical disclaimer section, security FAQ,
    and the full history of the guard — trim to what remains relevant once the guard is gone
  - **`#warning` directive** in `ble_config_service.cpp` that fires at compile time when
    the macro is defined
- Pure internal test infrastructure change — no user-facing docs need updating.
