---
id: TASK-138
title: Simulator — Long-Press and Double-Press Support
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: LongDoublePress
order: 7
prerequisites: [TASK-108]
---

## Description

Update the web simulator (`docs/simulator/simulator.js` and `docs/simulator/index.html`) to simulate long-press and double-press gestures, and to display `longPress` / `doublePress` sub-actions from the loaded `profiles.json`.

## Acceptance Criteria

- [ ] Button hold > 500 ms triggers the `longPress` action (if configured); short tap triggers the primary action
- [ ] Two clicks within 300 ms triggers the `doublePress` action (if configured); single click triggers the primary action
- [ ] Double-press takes priority — if confirmed, the primary action is NOT also fired
- [ ] Button label area shows a secondary indicator when `longPress` or `doublePress` is configured (e.g. a small `⟳` or `2×` badge)
- [ ] The BLE/IO output log prefixes long-press events with `[LONG]` and double-press with `[DBL]`
- [ ] Existing short-press, release, and delayed-action behaviour is unchanged
- [ ] Works in both mouse and touch event paths

## Files to Touch

- `docs/simulator/simulator.js` — timing logic, `simulateButtonPress`/`simulateButtonRelease`, label rendering, output formatting
- `docs/simulator/index.html` — any CSS or markup changes needed for the new badges

## Implementation Notes

**Long press:** Start a `setTimeout` on `mousedown`/`touchstart` for 500 ms. If the timer fires before `mouseup`, dispatch `longPress` and cancel the short-press path. On `mouseup` before 500 ms, clear the timer and proceed with normal short-press logic.

**Double press:** Track `lastTapTime` per button. On `mousedown`, if `Date.now() - lastTapTime < 300`, cancel any pending single-press timer and dispatch `doublePress`. Otherwise, start a short 300 ms timer before committing the single press (allows double-press window to expire first).

Thresholds should be defined as named constants at the top of `simulator.js`:
```js
const LONG_PRESS_MS = 500;
const DOUBLE_PRESS_WINDOW_MS = 300;
```

## Test Plan

Manual test in browser:

1. Load a profile with `longPress` configured on button A. Hold A > 500 ms → only `[LONG]` fires. Tap A quickly → only primary fires.
2. Load a profile with `doublePress` configured on button B. Double-tap B within 300 ms → only `[DBL]` fires. Single tap → only primary fires.
3. Verify existing profiles (no `longPress`/`doublePress`) behave identically to before.

## Notes

The 500 ms long-press threshold and 300 ms double-press window must match the firmware constants defined in `src/main.cpp` (`LONG_PRESS_THRESHOLD_MS = 500`) and `lib/hardware/esp32/include/button.h` (`doublePressWindow_ = 300`).
