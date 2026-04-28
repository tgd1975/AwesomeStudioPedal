---
id: TASK-112
title: Config Loader: Parse Macro Steps
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
epic: Macros
order: 3
prerequisites: [TASK-111, TASK-115]
---

## Description

Extend `config_loader` to recognise `"type": "Macro"` and parse the `steps` array into a fully-constructed `MacroAction`. Each step is a JSON array of action objects; the existing action-factory logic is called recursively per action.

## Acceptance Criteria

- [ ] Config loader handles `"type": "Macro"` and constructs a `MacroAction`
- [ ] `steps` array is parsed: each element is a JSON array of action objects
- [ ] Each action object within a step is parsed using the existing `parseAction()` factory (recursive)
- [ ] Resulting `MacroAction` has the correct step count and action types
- [ ] Missing or empty `steps` key produces an empty `MacroAction` (no crash)
- [ ] `data/profiles.json` updated with a macro example (see Notes)
- [ ] All existing tests still pass under `make test-host`

## Files to Touch

- `lib/PedalLogic/src/config_loader.cpp` — add `Macro` branch to action factory

## Test Plan

**Host tests** (`make test-host`): covered by TASK-114 (config loader round-trip test case).

## Prerequisites

- **TASK-111** — `MacroAction` class must exist before the loader can instantiate it
- **TASK-115** — `profiles.schema.json` defines the macro step shape; loader must match it

## Notes

Example macro JSON for `data/profiles.json`:

```json
"A": {
  "type": "Macro",
  "name": "Scene launch",
  "steps": [
    [{ "type": "PinHigh", "pin": 27 }],
    [{ "type": "SendKey", "value": "KEY_F1" }, { "type": "SendKey", "value": "KEY_F2" }],
    [{ "type": "Delayed", "delayMs": 500, "action": { "type": "PinLow", "pin": 27 } }]
  ]
}
```

Note: `"type": "Macro"` in JSON maps to `"MacroAction"` in `ACTION_TYPE_TABLE`. Ensure `lookupActionType` is called with the correct string form used in JSON (`"Macro"` short form or `"MacroAction"` — pick one and be consistent with the schema in TASK-115).
