---
id: TASK-293
title: Phase 3a — HostPedalApp fake; eliminate HOST_TEST_BUILD from logger / timing
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Medium (2-8h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 5
prerequisites: [TASK-292]
---

## Description

First slice of Phase 3 of EPIC-020. Two related goals:

1. **Bring up the `HostPedalApp` test fake.** Add
   `test/fakes/host_pedal_app.{h,cpp}` as the host-side specialization
   referenced in IDEA-028. It inherits directly from `PedalApp` (not
   `BlePedalApp`) and provides no-op platform hooks plus links against
   the existing fakes (`hardware_config_fake.cpp`, `NullLedController`,
   `NullLogger`). Wire it into `test/CMakeLists.txt` so test binaries
   can construct one if needed.
2. **Remove `#ifndef HOST_TEST_BUILD` from the simplest "logger /
   timing" guards.** Targets three files:
   - `lib/PedalLogic/src/serial_logger.cpp` (3 guards — Serial vs std::cout)
   - `lib/PedalLogic/src/delayed_action.cpp` (1 guard — Arduino include)
   - `lib/PedalLogic/src/non_send_action.cpp` (3 guards — Arduino include + `delay()` calls)

The pattern: extend `test/fakes/arduino_shim.h` (and/or introduce a
unified compat header) so that what the host build needs (a no-op
`delay()`, a `Serial`-like object that writes to `std::cout`) is
provided unconditionally for the host build, with no per-file
`#ifndef HOST_TEST_BUILD`. Files that include `<Arduino.h>` switch to
the compat header; the compat header internally selects the right
backend.

The `target_compile_definitions(pedal_tests PRIVATE HOST_TEST_BUILD)`
line in `test/CMakeLists.txt` stays in place for now — it still
gates the remaining guards in pin/serial/config_loader/littlefs.
Removing it entirely is TASK-295.

## Acceptance Criteria

- [ ] `test/fakes/host_pedal_app.h` and `host_pedal_app.cpp` exist;
      `HostPedalApp` inherits from `PedalApp` (not `BlePedalApp`).
- [ ] `test/CMakeLists.txt` builds `host_pedal_app.cpp` into the
      test binary. At least one host test that exercises a
      `PedalApp`-rooted code path (event handlers, profile manager
      glue) actually instantiates `HostPedalApp` instead of
      hand-rolled fakes.
- [ ] Zero `#ifndef HOST_TEST_BUILD` guards in:
      `lib/PedalLogic/src/serial_logger.cpp`,
      `lib/PedalLogic/src/delayed_action.cpp`,
      `lib/PedalLogic/src/non_send_action.cpp`.
- [ ] All builds clean: `pio run -e nodemcu-32s`,
      `pio run -e feather-nrf52840`. No new warnings.
- [ ] `make test-host` still passes (286 tests, or more if
      `HostPedalApp` integration adds new cases).

## Test Plan

**Host tests** (`make test-host`):

- The 286-test suite still passes after the guards are removed.
- Add a smoke test that constructs `HostPedalApp` and verifies the
  no-op platform hooks behave (file: `test/unit/test_host_pedal_app.cpp`).

**On-device** — no behaviour change expected; both production builds
must still link.

## Prerequisites

- **TASK-292** — establishes the `PedalApp` / `BlePedalApp` split
  that lets `HostPedalApp` inherit directly from `PedalApp`.

## Notes

- IDEA-028 §"Test / host specialization" sketches the
  `HostPedalApp` shape — selected at link time, not via
  `#ifdef HOST_TEST_BUILD`.
- The compat-header approach (extending `arduino_shim.h`) is
  cheaper than introducing full DI for `delay()` and `Serial`. DI
  may still be the right answer for Phase 3c/littlefs; for these
  three files the shim approach is enough.
- After this task, ~5 PedalLogic files still carry `#ifndef
  HOST_TEST_BUILD` guards — the next two slices clean those up.
