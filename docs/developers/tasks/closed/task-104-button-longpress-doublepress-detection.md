---
id: TASK-104
title: Button Long-Press and Double-Press Detection
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: LongDoublePress
order: 1
---

## Description

Extend `IButton` and the ESP32 `Button` implementation to detect two new trigger events: a long press (button held beyond a configurable threshold) and a double press (two presses within a configurable window). These are the foundation for TASK-105 through TASK-109.

The nRF52840 `Button` is out of scope for this sprint — it must compile but may return stubs.

## Acceptance Criteria

- [ ] `IButton` gains two new pure-virtual methods: `holdDurationMs()` and `doublePressEvent()`
- [ ] ESP32 `Button` records `pressStartTime_` on falling edge in `isr()`
- [ ] `holdDurationMs()` returns `millis() - pressStartTime_` while `awaitingRelease == true`, `0` otherwise
- [ ] `doublePressEvent()` returns `true` once per confirmed double press (two presses within `doublePressWindow_`, default 300 ms) and clears the flag
- [ ] `setDoublePressWindow(ms)` allows overriding the default window
- [ ] nRF52840 `Button` stub compiles: `holdDurationMs()` returns `0`, `doublePressEvent()` returns `false`
- [ ] `doublePressEvent()` takes priority — if a double press is confirmed, the single-press `event()` must NOT also fire for the same interaction
- [ ] All existing host tests still pass under `make test-host`

## Files to Touch

- `lib/PedalLogic/include/i_button.h` — add pure-virtual `holdDurationMs()` and `doublePressEvent()`
- `lib/hardware/esp32/include/button.h` — add `pressStartTime_`, `doublePressFlag_`, `doublePressWindow_`, `lastPressTime_`
- `lib/hardware/esp32/src/button.cpp` — implement detection logic in `isr()` and accessors
- `lib/hardware/nrf52840/include/button.h` — stub implementations
- `lib/hardware/nrf52840/src/button.cpp` — stub implementations (if needed)

## Test Plan

**Host tests** (`make test-host`): covered by TASK-108.

## Prerequisites

None — can start independently within Group B.

## Notes

`doublePressEvent()` must be checked before `event()` in the main loop (see TASK-107). The ISR rising-edge timestamp comparison drives double-press detection: on each rising edge (release), if `(millis() - lastPressTime_) < doublePressWindow_` set `doublePressFlag_` and suppress the pending single-press event. `lastPressTime_` is updated on every falling edge (press).
