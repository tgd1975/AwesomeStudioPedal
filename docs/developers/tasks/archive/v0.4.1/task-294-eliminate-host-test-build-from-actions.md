---
id: TASK-294
title: Phase 3b — eliminate HOST_TEST_BUILD from action implementations
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 6
prerequisites: [TASK-293]
---

## Description

Second slice of Phase 3. Apply the compat-header pattern from
TASK-293 to the remaining "leaf" action and config files:

- `lib/PedalLogic/src/pin_action.cpp` (1 guard — Arduino include
  for `digitalWrite`/`HIGH`/`LOW`)
- `lib/PedalLogic/src/serial_action.cpp` (2 guards — include +
  `Serial.print` body)
- `lib/PedalLogic/src/ble_config_reassembler.cpp` (2 guards —
  include + default-init for delay/millis callbacks)
- `lib/PedalLogic/src/config_loader.cpp` (1 guard — Arduino
  include)

After this task, the only remaining guard is
`lib/PedalLogic/src/littlefs_file_system.cpp`, plus the
`HOST_TEST_BUILD` define in `test/CMakeLists.txt`. Both go in
TASK-295.

## Acceptance Criteria

- [ ] Zero `#ifndef HOST_TEST_BUILD` guards in:
      `pin_action.cpp`, `serial_action.cpp`,
      `ble_config_reassembler.cpp`, `config_loader.cpp`.
- [ ] `arduino_shim.h` provides whatever GPIO / Serial-like
      shims these files need (extended in TASK-293; this task
      may add small extensions for `digitalWrite` if not yet
      covered).
- [ ] `pio run -e nodemcu-32s` and `pio run -e feather-nrf52840`
      build clean.
- [ ] `make test-host` still passes.

## Test Plan

**Host tests** (`make test-host`):

- The full suite still passes. Test files that exercise
  `PinAction` (`test_pin_action.cpp`),
  `SerialOutputAction` (`test_serial_action.cpp`), and the
  `BleConfigReassembler` flow are particularly important — each
  was previously compiling against a guarded code path; verify
  they still compile and pass with the guards removed.

**On-device** — no behaviour change expected.

## Prerequisites

- **TASK-293** — establishes the compat-header pattern and the
  initial `arduino_shim.h` extensions (no-op `delay`,
  `Serial`-like). This task reuses that infrastructure.

## Notes

- `ble_config_reassembler.cpp` already takes `delay` and `millis`
  as constructor callbacks — its guard is just for default-init
  fallback when callbacks are not supplied. The simplest fix:
  always require callers to supply them (no default), and let the
  host tests pass test-fixture lambdas. Production callers
  (`BleConfigService` on ESP32) already supply them.
- `pin_action.cpp`'s `digitalWrite`/`HIGH`/`LOW` map cleanly to
  no-ops or fake-state shims in `arduino_shim.h`. If host tests
  want to assert on the GPIO state, the shim can record calls.
- After this task: only `littlefs_file_system.cpp` remains, and
  the `HOST_TEST_BUILD` compile-definition can be dropped in
  TASK-295.
