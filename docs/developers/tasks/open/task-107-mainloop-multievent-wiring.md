---
id: TASK-107
title: Wire Multi-Event Dispatch in main.cpp
status: open
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: LongDoublePress
order: 4
prerequisites: [TASK-106]
---

## Description

Update `src/main.cpp` `loop()` to poll `doublePressEvent()`, `holdDurationMs()`, and `event()` in the correct priority order and dispatch the appropriate event via `EventDispatcher`. Double press takes priority over single press; long press fires once per hold.

## Acceptance Criteria

- [ ] `doublePressEvent()` is checked before `event()` — if true, dispatch double press and skip single press
- [ ] `holdDurationMs() >= LONG_PRESS_THRESHOLD_MS` fires long press exactly once per hold (guarded by `longPressArmed[]`)
- [ ] `longPressArmed[i]` is reset to `false` when `awaitingRelease` goes low
- [ ] `LONG_PRESS_THRESHOLD_MS` defined as `constexpr uint32_t` in `main.cpp` (default 500 ms); overridable in `config.h`
- [ ] `longPressArmed` array sized to `hardwareConfig.numButtons`
- [ ] Existing single-press and release behaviour is unchanged for buttons with no long/double press registered
- [ ] Firmware builds without warnings for ESP32 target

## Files to Touch

- `src/main.cpp` — update `loop()`, add `longPressArmed[]` array and threshold constant

## Test Plan

Manual smoke test on device: hold button > 500 ms → only long-press action fires. Double-tap within 300 ms → only double-press fires. Single tap → only single-press fires.

**Host tests** (`make test-host`): covered by TASK-108.

## Prerequisites

- **TASK-106** — `Profile` must expose `getLongPressAction` / `getDoublePressAction` before `main.cpp` can read them

## Notes

Suggested loop snippet:

```cpp
if (btn->doublePressEvent())
    eventDispatcher.dispatchDoublePress(i);
else if (btn->event())
    eventDispatcher.dispatch(i);

if (btn->holdDurationMs() >= LONG_PRESS_THRESHOLD_MS && !longPressArmed[i]) {
    eventDispatcher.dispatchLongPress(i);
    longPressArmed[i] = true;
}
if (!btn->awaitingRelease) longPressArmed[i] = false;
```
