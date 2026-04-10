---
id: TASK-061
title: Fix LED blink trigger on profile switch
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Medium
human-in-loop: Support
---

## Description

The LED array blinks on every profile switch (SELECT button press). According to the intended
design, blinking should only occur when a new configuration has been ingested (loaded from
storage/serial), not on a normal profile switch. A normal switch should immediately show the
LED encoding for the new profile without any blink animation.

## Acceptance Criteria

- [ ] Pressing the SELECT button cycles profiles with no blink — LEDs update immediately to
      the encoding of the new active profile
- [ ] Loading a new configuration (config ingestion path) still triggers the blink animation
- [ ] Existing host unit tests updated/extended to reflect the new behaviour
- [ ] Manually verified on device

## Notes

Current behaviour: `switchProfile()` sets `postSwitchBlink = true`, triggering the 3-blink
sequence on every call regardless of how the switch was initiated.

The fix likely involves removing `postSwitchBlink = true` from `switchProfile()` and instead
triggering it only from the config-ingestion code path (wherever a new config is applied to
the `ProfileManager` — probably `setCurrentProfile()`, `resetToFirstProfile()`, or the
config loader's post-load callback).

Investigate where config ingestion ends and which method is called on `ProfileManager` after
a successful load, then add a dedicated `triggerBlink()` method (or a boolean parameter) so
only that path triggers the blink.

Related: TASK-047 (fixed boot-time sentinel bug in the blink state machine — that fix
should be kept; only the trigger condition changes).
