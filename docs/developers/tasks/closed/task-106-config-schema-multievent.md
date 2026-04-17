---
id: TASK-106
title: Config Schema Extension for Multi-Event Bindings
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: LongDoublePress
order: 3
prerequisites: [TASK-105]
---

## Description

Extend the config loader and `Profile` to support optional `longPress` and `doublePress` sub-action objects on each button config entry. Existing single-action profiles are unchanged — both keys are optional.

## Acceptance Criteria

- [ ] `Profile` stores long-press and double-press actions per button alongside the primary action
- [ ] `Profile::addLongPressAction(uint8_t button, std::unique_ptr<Action>)` accessor added
- [ ] `Profile::addDoublePressAction(uint8_t button, std::unique_ptr<Action>)` accessor added
- [ ] `Profile::getLongPressAction(uint8_t button)` and `getDoublePressAction(uint8_t button)` added (return `nullptr` if absent)
- [ ] `config_loader` parses `longPress` and `doublePress` keys recursively using the existing action-factory logic
- [ ] Missing keys produce `nullptr` — no error, no crash
- [ ] Example `data/profiles.json` updated with at least one button demonstrating `longPress`
- [ ] All existing tests still pass under `make test-host`

## Files to Touch

- `lib/PedalLogic/include/profile.h` — new accessors and storage
- `lib/PedalLogic/src/profile.cpp` — implement accessors
- `lib/PedalLogic/src/config_loader.cpp` — parse `longPress` / `doublePress` keys
- `data/profiles.json` — add one example

## Test Plan

**Host tests** (`make test-host`): covered by TASK-108 (`test_action_parsing.cpp` / `test_config_loader.cpp`).

## Prerequisites

- **TASK-105** — dispatcher API must exist so the loader has a destination for the new actions

## Notes

The sub-action objects use the exact same JSON structure as the primary action. The config loader should call the existing `parseAction()` (or equivalent factory) recursively on both sub-objects. Button config example:

```json
"A": {
  "type": "SendKey", "value": "KEY_F1", "name": "Play",
  "longPress":   { "type": "SendMediaKey", "value": "MEDIA_STOP",  "name": "Stop" },
  "doublePress": { "type": "SendKey",      "value": "KEY_F2",       "name": "Record" }
}
```
