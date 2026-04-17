---
id: TASK-080
title: Test coverage audit
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Large (8-24h)
complexity: Senior
human-in-loop: Support
group: Finalization
order: 4
---

## Description

Audit host and on-device test coverage across the entire codebase before the first public
release. Identify untested logic paths in `lib/PedalLogic/`, uncovered action types in
`config_loader.cpp`, and hardware-interaction paths that lack on-device tests. Add the missing
tests and ensure the coverage baseline is documented so regressions can be caught in CI.

## Acceptance Criteria

- [ ] Every action type (`SendChar`, `SendString`, `SendMediaKey`, `SerialOutput`, `DelayedAction`,
      all pin variants) has at least one host-unit test exercising its dispatch path
- [ ] `config_loader.cpp` is covered for valid configs, missing fields, unknown action types,
      and malformed JSON
- [ ] On-device test suite covers button press → BLE output round-trip and profile switching
      on real ESP32 hardware; documented in the task or PR description

## Test Plan

**Host tests** (`make test-host`):

- Extend `test/unit/` with tests for any uncovered action types and edge cases in
  `config_loader.cpp`
- Register any new files in `test/CMakeLists.txt`
- Cover: all action type dispatches, null/missing action, profile index out of range,
  malformed JSON graceful failure

**On-device tests** (`make test-esp32-button` / `/test-device`):

- Verify button-press → BLE HID output and SELECT → profile LED change on real hardware
- Requires: ESP32 connected via USB

## Notes

- Run `/test` first to see current pass/fail baseline; use that as the starting coverage map.
- The `HOST_TEST_BUILD` guard pattern (see `test/fakes/arduino_shim.h`) allows shimming most
  Arduino API calls — prefer host tests wherever possible.
- If coverage tooling (lcov / gcov) is not yet wired into CI, consider adding it as a
  sub-task or a follow-up.
- Coordinate with TASK-079 (senior code review) — reviewers may surface untested paths.
