---
id: TASK-047
title: Fix profile LED blinking behavior
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Support
---

## Description

After a profile switch, the LED does not blink correctly to indicate the active profile number.
Investigate the LED blinking logic and fix the behavior so it matches the expected pattern.

## Acceptance Criteria

- [x] LED blinks the correct number of times after a profile switch
- [x] Behavior is consistent across all profiles
- [x] Existing host unit tests pass
- [x] New unit test added covering the blink-after-switch scenario

## Notes

Root cause: `blinkStartTime = 0` was used as a sentinel value for "blink not yet started".
If `update()` was called with `now = 0` (possible at boot when `millis()` returns 0), the
sentinel check `blinkStartTime == 0` remained true on every subsequent call, causing the blink
to re-initialise infinitely and never complete.

Fix: added a dedicated `blinkStarted` boolean flag to `profile_manager.h` and updated
`profile_manager.cpp` to use `!blinkStarted` instead of `blinkStartTime == 0`.
`blinkStarted` is reset to `false` in `switchProfile()` alongside `blinkPhase`.

Added regression test `PostSwitchBlink_WorksWhenNowIsZero` in
`test/unit/test_profile_manager.cpp` — calls `update(0)` then `update(5000)` and verifies
the LEDs end up in the correct profile-encoding state after the blink completes.

Related: TASK-046 (fix profile select button) introduced the blink sequence; this fixes
the sentinel bug that made it unreliable at boot.
