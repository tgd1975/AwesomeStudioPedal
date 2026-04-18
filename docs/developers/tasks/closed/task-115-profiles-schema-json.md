---
id: TASK-115
title: Create profiles.schema.json and Pre-Commit Validation
status: closed
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: BLEConfig
order: 1
---

## Description

Create a JSON Schema (draft-07) for `data/profiles.json` and wire it into the pre-commit hook so every commit validates the live profiles file. This schema is a prerequisite for the config loader macro extension (TASK-112), the Python CLI `validate` subcommand (TASK-119), and the Flutter app's offline validation (TASK-123).

## Acceptance Criteria

- [ ] `data/profiles.schema.json` created as a valid JSON Schema draft-07 document
- [ ] Schema covers: top-level `profiles` array; each entry requires `name` (string) and `buttons` (object)
- [ ] Each button value is an **action object**: required `type` field (enum of all action type strings), optional `name`, type-specific required fields (`value` for Send actions, `pin` for Pin actions, `delayMs` + `action` for Delayed, `steps` array of arrays for Macro)
- [ ] `longPress` and `doublePress` keys on button objects use a `$ref` to the same action object schema (self-referential)
- [ ] Macro `steps`: array of arrays of action objects
- [ ] `docs/data/README.md` updated to reference the schema
- [ ] `scripts/pre-commit` (or equivalent hook) validates `data/profiles.json` against the schema using `jsonschema` (Python) or `ajv` (Node) — whichever is available in the dev container
- [ ] `data/profiles.json` passes validation after the hook is added
- [ ] All existing tests still pass under `make test-host`

## Files to Touch

- `data/profiles.schema.json` (new)
- `docs/data/README.md` (updated)
- `scripts/pre-commit` or `.git/hooks/pre-commit` — add schema validation step

## Test Plan

Run the pre-commit hook manually against a known-bad `profiles.json` and verify it rejects it. Run against the current `data/profiles.json` and verify it passes.

## Prerequisites

None — no dependency on other tasks. This is the first Group D task and a prerequisite for C3, D0b, D3, D4, and E1.

## Notes

Action type enum values in the schema should match the strings used in `ACTION_TYPE_TABLE` in `key_lookup.cpp`: `SendStringAction`, `SendCharAction`, `SendKeyAction`, `SendMediaKeyAction`, `SerialOutputAction`, `DelayedAction`, `PinHighAction`, `PinLowAction`, `PinToggleAction`, `PinHighWhilePressedAction`, `PinLowWhilePressedAction`, `MacroAction`.

Self-referential action object using `$ref`:
```json
"$defs": {
  "actionObject": {
    "type": "object",
    "required": ["type"],
    "properties": {
      "type": { "type": "string", "enum": ["SendKeyAction", ...] },
      "longPress":   { "$ref": "#/$defs/actionObject" },
      "doublePress": { "$ref": "#/$defs/actionObject" }
    }
  }
}
```
