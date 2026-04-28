---
id: TASK-102
title: Implement getJsonProperties on PinAction
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: IOTestrig
order: 2
prerequisites: [TASK-101]
---

## Description

`Action::getJsonProperties` is a virtual no-op on the base class. `PinAction` should override it to emit `{ "pin": <N> }` so that future CLI export and config-builder round-trips can reconstruct the full action from JSON. Add a corresponding host test verifying the output.

## Acceptance Criteria

- [ ] `PinAction::getJsonProperties` overrides the base no-op and emits the correct `"pin"` key
- [ ] Test case in `test/unit/test_pin_action.cpp` verifies the emitted value
- [ ] All tests pass under `make test-host`

## Test Plan

**Host tests** (`make test-host`):

- Add `TEST_F(PinActionTest, GetJsonProperties_EmitsPin)` in `test/unit/test_pin_action.cpp`
- Cover: pin value is correct; type string is handled separately by `ProfileManager::getActionTypeString`

## Prerequisites

- **TASK-101** — prior test gap fill ensures the test file is in a clean state before adding more cases

## Notes

The type string is already handled by `ProfileManager::getActionTypeString` and does not need to be emitted here. This output enables the Python CLI tool (TASK-119) to export action configs to JSON.
