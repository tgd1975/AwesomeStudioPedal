---
id: TASK-105
title: EventDispatcher Multi-Event API
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: LongDoublePress
order: 2
prerequisites: [TASK-104]
---

## Description

Extend `EventDispatcher` with registration and dispatch methods for long-press and double-press events. The existing single-press API (`registerHandler` / `dispatch`) is unchanged — new methods are additive.

## Acceptance Criteria

- [ ] `registerLongPressHandler(uint8_t button, EventCallback cb, uint32_t thresholdMs = 500)` added
- [ ] `registerDoublePressHandler(uint8_t button, EventCallback cb)` added
- [ ] `dispatchLongPress(uint8_t button)` added — calls the registered long-press callback for that button
- [ ] `dispatchDoublePress(uint8_t button)` added — calls the registered double-press callback for that button
- [ ] A `longPressThreshold_` map or array stores per-button thresholds
- [ ] Unregistered buttons are silently ignored (no crash)
- [ ] All existing tests still pass under `make test-host`

## Files to Touch

- `lib/PedalLogic/include/event_dispatcher.h` — new method declarations, new member storage
- `lib/PedalLogic/src/event_dispatcher.cpp` — implementations

## Test Plan

**Host tests** (`make test-host`): covered by TASK-108.

## Prerequisites

- **TASK-104** — `IButton` interface must be extended before the dispatcher can reference threshold logic

## Notes

Long-press dispatch is called from `loop()` (TASK-107) when `holdDurationMs() >= threshold` and not yet fired for this hold. A `longPressArmed_` flag per button (stored in `main.cpp`, not here) prevents repeated firing while held. This class only exposes the registration and dispatch primitives.
