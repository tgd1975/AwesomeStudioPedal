---
id: TASK-076
title: Verify and fix delayed action polling in main loop
status: closed
closed: 2026-04-16
opened: 2026-04-16
effort: Small (<2h)
complexity: Medium
human-in-loop: No
---

## Description

`DelayedAction` has a `update(uint32_t currentTime)` method that returns `true` when the delay has
elapsed, but it is never called from `loop()` in `main.cpp`. The current flow is:

1. Button press → `execute()` starts the timer and sets `started = true`.
2. `isInProgress()` returns `true`, so all subsequent button presses are silently ignored.
3. `update()` is never polled, so the inner action never fires.

The delayed action is effectively a dead-end: it starts but never completes. The loop already
detects active delayed actions (via `hasActiveDelayedAction()`) and blinks the power LED, but
never actually fires the inner action when the timer expires.

The fix is to iterate over all active buttons in `loop()`, call `DelayedAction::update(now)` on
any action that `isInProgress()`, and call the inner action directly when `update()` returns
`true`. The `DelayedAction::getInnerAction()` accessor and `Action::execute()` are already in
place. The fix must be non-blocking — no `delay()` calls in the polling path.

## Acceptance Criteria

- [ ] `loop()` polls each button's action for expired delays on every iteration (non-blocking).
- [ ] When a `DelayedAction`'s timer expires, its inner action fires exactly once and `started`
      resets to `false` (power LED blink stops).
- [ ] A unit test covers the end-to-end path: press → delay elapses → inner action fires.
- [ ] Pressing the button while a delay is in progress remains ignored (existing guard kept).

## Notes

The relevant code paths are:

- `lib/PedalLogic/include/delayed_action.h` — `update()`, `getInnerAction()`, `isInProgress()`
- `lib/PedalLogic/src/delayed_action.cpp` — `execute()`, `update()`
- `src/main.cpp` — `loop()`, `executeActionWithLogging()`, `hasActiveDelayedAction()`

The polling loop must access the action via `profileManager->getAction(currentProfile, buttonIdx)`
and cast to `DelayedAction*` only if `getType() == Action::Type::Delayed`.
