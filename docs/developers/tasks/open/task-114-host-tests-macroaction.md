---
id: TASK-114
title: Host Tests for MacroAction
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: Macros
order: 5
prerequisites: [TASK-113]
---

## Description

Write comprehensive GoogleTest host tests for `MacroAction` covering step execution, sequencing, in-progress state, release propagation, and config-loader round-trips.

## Acceptance Criteria

- [ ] Single-step macro with one action: fires on `execute()`, `isInProgress()` returns `false` immediately after (synchronous action)
- [ ] Single-step macro with two parallel actions: both fire on `execute()`
- [ ] Two-step sequential macro: step 2 does not fire until step 1 completes (`isInProgress()` is `true` in between, `update()` advances it)
- [ ] Macro with a `DelayedAction` step: does not advance until fake time exceeds delay threshold
- [ ] `executeRelease()` propagates to all actions in the currently active step
- [ ] `getType()` returns `Action::Type::Macro`
- [ ] Config loader round-trip: parse a three-step macro JSON, verify action types and step counts match
- [ ] All tests pass under `make test-host`

## Files to Touch

- `test/unit/test_macro_action.cpp` (new)
- `test/CMakeLists.txt` — add `test_macro_action.cpp` to test sources

## Test Plan

Run `make test-host` — all new and existing cases must pass.

## Prerequisites

- **TASK-113** — full macro feature stack must be implemented before tests are written

## Notes

Use the fake-time pattern from `arduino_shim.h` (`HOST_TEST_BUILD`) to advance `millis()` past the `DelayedAction` threshold in the delay-step test case. Create stub actions (e.g. `MockAction` with a manually-settable `isInProgress()`) using GoogleTest mocks or a simple hand-written test double.
