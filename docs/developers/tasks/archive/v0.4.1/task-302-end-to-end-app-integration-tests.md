---
id: TASK-302
title: Phase 4g — end-to-end PedalApp integration tests on host
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 14
prerequisites: [TASK-301]
---

## Description

First crop of integration tests that exercise the full
`BlePedalApp::setup()` / `loop()` flow end-to-end on host, using
`HostPedalApp` + `MockBleKeyboard` + per-pin `fake_gpio` from the
preceding tasks. These tests cement the value the HAL refactor was
supposed to unlock — being able to test pedal behaviour without
flashing hardware.

Add `test/unit/test_app_integration.cpp` with 3–5 tests:

1. **Boot path** — construct `HostPedalApp` with a `MockBleKeyboard`,
   call `setup()`, verify no crash, BLE keyboard `begin()` called.
2. **Button press → SendKey action** — press button A (pin from
   `hardwareConfig.buttonPins[0]`), call `loop()` once, verify
   `MockBleKeyboard.write()` was called with the configured key.
3. **Profile select roundtrip** — press SELECT button, verify
   `saveProfile` was called and `loadProfile` returns the new
   index on a fresh app instance (using the in-memory store
   added in TASK-300).
4. **Long-press dispatch** — press button A, advance `fake_time`
   past `LONG_PRESS_THRESHOLD_MS` (500ms), call `loop()` enough
   times for the long-press to register, verify the long-press
   action fired (not the regular press action).
5. **Delayed-action timing** *(optional)* — trigger a delayed
   action, verify it does not fire until time advances past the
   delay.

These tests live alongside the existing 287 unit tests but exercise
a richer slice of the system. They should run fast (no real I/O)
and deterministic (controlled time, mock outputs).

## Acceptance Criteria

- [ ] `test/unit/test_app_integration.cpp` exists and is registered
      in `test/CMakeLists.txt`.
- [ ] At least 4 of the 5 scenarios above are covered.
- [ ] Each test uses a fresh `HostPedalApp` instance and resets
      `fake_gpio` and `fake_time` between tests.
- [ ] `make test-host` passes (existing 287 + at least 4 new = 291+).
- [ ] Test failure modes are useful: a test that breaks because of
      a real regression (e.g. event dispatcher mis-wired) produces
      a recognizable assertion message, not just "loop() returned".

## Test Plan

This task is the test plan for the broader EPIC-020 refactor: it
proves the HAL enables hardware-independent integration testing.
No additional manual / on-device verification.

## Prerequisites

- **TASK-301** — per-pin `fake_gpio` so multiple buttons can be
  pressed independently.

## Notes

- These tests will likely surface small wiring assumptions in
  `BlePedalApp::setup()` that depend on real Arduino behaviour
  (e.g. `Serial.begin()` blocking, `delay()` in `setup()`). The
  arduino_shim makes those no-ops; tests should still pass but
  watch for hidden time-dependence.
- `MockBleKeyboard` (already in `test/mocks/`) needs to expose
  the recorded `write()` calls in an assertable way. If the
  current API isn't sufficient, extend it minimally (don't
  rewrite — that's a separate concern).
- After this task, the EPIC-020 implementation is fully done.
  Phase 5 (docs) is the only remaining phase.
