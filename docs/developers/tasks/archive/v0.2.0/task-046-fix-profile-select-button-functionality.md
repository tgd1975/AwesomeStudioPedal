---
id: TASK-046
title: Fix profile select button functionality
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
---

## Description

The profile select button was not correctly cycling through profiles or was triggering
unintended actions. Investigate and fix the button handling logic.

## Acceptance Criteria

- [x] Profile select button cycles through profiles correctly
- [x] No unintended actions triggered on button press
- [x] Behaviour verified on ESP32 hardware
- [x] Host unit tests pass

## Notes

Delivered in commit `4a8f8d6` along with TASK-047 (LED blink fix) and TASK-048 (persistent storage).
The fix addressed both debouncing and state machine logic in the button handler.
