---
id: TASK-101
title: Audit and Fill PinAction Host Test Gaps
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: IOTestrig
order: 1
---

## Description

Close coverage gaps on the recently added `PinAction`. Several scenarios are not yet exercised by the existing `test/unit/test_pin_action.cpp` and `test/unit/test_action_parsing.cpp`. No production code changes required — only new `TEST_F` cases.

## Acceptance Criteria

- [ ] `PinAction` name round-trip test added (`setName` / `getName` survives construction)
- [ ] `PinToggle` per-instance state test added (two independent objects toggle independently)
- [ ] Config loader tests added: all five pin types parse from JSON with correct `getType()`
- [ ] Config loader test added: missing or negative `pin` field returns `nullptr`
- [ ] All new tests pass under `make test-host`

## Test Plan

**Host tests** (`make test-host`):

- Extend `test/unit/test_pin_action.cpp` with name round-trip and per-instance toggle cases
- Extend `test/unit/test_action_parsing.cpp` with five-type parse and missing-pin rejection cases

## Notes

These gaps were identified during the sprint planning review. No dependency on other tasks — can be the first task started in Group A.
