---
id: TASK-047
title: Fix profile LED blinking behavior
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Support
---

## Description

After a profile switch, the LED does not blink correctly to indicate the active profile number.
Investigate the LED blinking logic and fix the behavior so it matches the expected pattern.

## Acceptance Criteria

- [ ] LED blinks the correct number of times after a profile switch
- [ ] Behavior is consistent across all profiles
- [ ] Existing host unit tests pass
- [ ] New unit test added covering the blink-after-switch scenario

## Notes

Related to TASK-046 (fix profile select button) which was closed — check if that fix introduced
the regression or if this is a pre-existing separate issue.
Hardware: ESP32 with `ILEDController` interface. Host test build uses `HOST_TEST_BUILD` guard.
