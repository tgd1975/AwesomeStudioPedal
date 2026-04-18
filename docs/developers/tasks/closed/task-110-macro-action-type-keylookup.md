---
id: TASK-110
title: Add Macro to Action::Type and key_lookup
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Trivial (<30m)
complexity: Junior
human-in-loop: No
group: Macros
order: 1
---

## Description

Register `Action::Type::Macro` in the enum and add `"Macro"` to `lookupActionType()` so the config loader can resolve it by string name.

> **Note:** As of 2026-04-17 both changes are already present in the codebase (`action.h` line 28, `key_lookup.cpp` line 173). Verify before doing any work — this task may be a no-op.

## Acceptance Criteria

- [ ] `Action::Type::Macro` exists in `lib/PedalLogic/include/action.h`
- [ ] `"MacroAction"` → `Action::Type::Macro` entry exists in `ACTION_TYPE_TABLE` in `lib/PedalLogic/src/key_lookup.cpp`
- [ ] `lookupActionType("MacroAction")` returns `Action::Type::Macro` (verified by a test or manual check)
- [ ] All existing tests pass under `make test-host`

## Files to Touch

- `lib/PedalLogic/include/action.h` — add `Macro` to enum (if not present)
- `lib/PedalLogic/src/key_lookup.cpp` — add entry to `ACTION_TYPE_TABLE` (if not present)

## Test Plan

Check existing tests pass. If adding new entries, extend `test/unit/test_key_lookup.cpp` (or equivalent) with a `lookupActionType("MacroAction")` assertion.

## Prerequisites

None — no dependency on other tasks.

## Notes

Already implemented on this branch. Confirm with a quick `grep` before touching files:
- `grep -n "Macro" lib/PedalLogic/include/action.h`
- `grep -n "MacroAction" lib/PedalLogic/src/key_lookup.cpp`
