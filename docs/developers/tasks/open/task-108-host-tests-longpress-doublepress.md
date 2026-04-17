---
id: TASK-108
title: Host Tests for Long Press and Double Press
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: LongDoublePress
order: 5
prerequisites: [TASK-107]
---

## Description

Add host-side GoogleTest coverage for the long-press/double-press feature stack: `Button` timing logic, `EventDispatcher` multi-event dispatch, and the config loader's `longPress`/`doublePress` parsing.

## Acceptance Criteria

- [ ] `Button` — `holdDurationMs()` returns `0` before press, non-zero while held, `0` after release
- [ ] `Button` — `doublePressEvent()` fires exactly once for two presses within the window; does not fire for single press; does not fire for two presses outside the window
- [ ] `Button` — `doublePressEvent()` firing suppresses the single-press `event()` for the same interaction
- [ ] `EventDispatcher` — long-press handler fires exactly once per hold regardless of how long the button stays held
- [ ] `EventDispatcher` — double-press handler fires; single-press handler does NOT also fire
- [ ] `EventDispatcher` — unregistered button index triggers no crash
- [ ] `config_loader` — parses `longPress` sub-object into correct action type
- [ ] `config_loader` — parses `doublePress` sub-object into correct action type
- [ ] `config_loader` — missing `longPress` / `doublePress` keys produce `nullptr` without error
- [ ] All tests pass under `make test-host`

## Files to Touch

- `test/unit/test_button.cpp` — new `TEST_F` cases for timing and double-press logic
- `test/unit/test_event_dispatcher.cpp` — new cases for `registerLongPressHandler` / `registerDoublePressHandler`
- `test/unit/test_action_parsing.cpp` — new cases for `longPress` / `doublePress` parsing
- `test/CMakeLists.txt` — ensure all affected source files are registered (likely already present)

## Test Plan

Run `make test-host` — all new and existing cases must pass.

## Prerequisites

- **TASK-107** — full feature stack must be implemented before comprehensive tests are written

## Notes

Use the existing `arduino_shim.h` fake-time pattern (`HOST_TEST_BUILD` guard) to control `millis()` in `Button` tests. Advance fake time past the long-press threshold to verify the guard, and keep it within the double-press window to verify detection.
