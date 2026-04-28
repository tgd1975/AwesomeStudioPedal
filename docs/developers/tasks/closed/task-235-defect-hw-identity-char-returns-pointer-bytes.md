---
id: TASK-235
title: Defect — Hardware-identity BLE characteristic returned 4 bytes of pointer address instead of the string
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: No
epic: feature_test
order: 23
---

## Description

`BleConfigService::setupGattService` in
[lib/hardware/esp32/src/ble_config_service.cpp](../../../../lib/hardware/esp32/src/ble_config_service.cpp)
was calling:

```cpp
NimBLECharacteristic* hwChar =
    svc->createCharacteristic(CHAR_HW_IDENTITY_UUID, NIMBLE_PROPERTY::READ);
hwChar->setValue(hardwareConfig.hardware);
```

`hardwareConfig.hardware` is `const char*`. NimBLE-Arduino exposes two
relevant `setValue` paths:

- `setValue(const uint8_t*, size_t)` — explicit raw-bytes overload.
- `template<typename T> setValue(const T&)` — template.

The template, when `T = const char*`, SFINAE-selects the generic
`sizeof(T)` branch (because `const char*` has no `c_str()` method) and
writes `(uint8_t*)&s, sizeof(T)` — i.e., the 4 bytes of the **pointer
itself**, not the string it points to. The characteristic therefore
always returned 4 bytes of essentially-random pointer-address data to
any GATT client that read it.

### Impact

- `pedal_config.py upload-config` reads this characteristic to verify
  the config's `hardware` field matches the compiled firmware target
  (TASK-180 safety guard). Every call failed on production firmware:

  ```
  ERROR: hardware mismatch: config targets 'esp32' but connected device is '\xff\xff@?'.
  ```

- Any app-side read-back of the same characteristic saw the same
  garbage.
- The safety guard therefore always triggered, regardless of whether
  the config was actually correct — a pure false positive.

### Root cause

NimBLE's two string-friendly `setValue` overloads both require a wrapper
type:

- `setValue(const uint8_t*, size_t)` — need to cast and pass `strlen`.
- `setValue(const std::string&)` — takes the `c_str()/length()` branch.

Neither is selected automatically for a bare `const char*`. The only
"just works" path is via `std::string`.

## Fix applied

```cpp
hwChar->setValue(std::string(hardwareConfig.hardware));
```

Added `#include <string>` in the same TU for clarity.

## Acceptance Criteria

- [x] Characteristic returns the correct hardware identity string on a
      GATT read.
- [x] `pedal_config.py upload-config data/config.json` completes with
      `Hardware identity verified: esp32`, then `OK: upload successful`.
- [x] No regressions in the on-device BLE integration test (test
      firmware does not use this path).

## Test Plan

**On-device manual verification** (already performed as part of
TASK-151 / C-01):

- Reflash production firmware. Run `pedal_config.py upload-config
  data/config.json`. Confirm `Hardware identity verified: esp32`.

## Notes

- Discovered during TASK-151 C-01 execution (the empirical manifestation
  was "hardware mismatch: …'\xff\xff@?'").
- Consider a host-side round-trip test that mocks the NimBLE
  characteristic layer and asserts `setValue` with `const char*` produces
  the expected bytes. The bigger lesson: "passing a `const char*` to a
  templated `setValue` silently stores pointer bytes" is a library
  footgun; a lint or static wrapper could catch it project-wide.
