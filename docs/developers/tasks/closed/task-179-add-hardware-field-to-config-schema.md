---
id: TASK-179
title: Add hardware field to config.json and schema
status: closed
opened: 2026-04-20
effort: Small (<2h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: hardware-selector
order: 1
---

## Description

The `data/config.json` file and its JSON Schema (`data/config.schema.json`) currently
contain no field that identifies which microcontroller board the configuration targets.
This means a config built for the nRF52840 is indistinguishable from one built for the
ESP32 — they share the same field names but use completely different GPIO pin numbers.

Add a required `"hardware"` enum field to both the schema and the live config file.
This field is the foundation for all downstream safety checks (firmware boot guard,
CLI upload validation, app upload guard) and for the tooling that pre-fills
board-specific defaults.

## Acceptance Criteria

- [ ] `data/config.schema.json` contains `"hardware"` as a required property with
  `"enum": ["esp32", "nrf52840"]` and no default (omitting it must be a schema error)
- [ ] `data/config.json` contains `"hardware": "esp32"` (it documents the ESP32 wiring)
- [ ] `pre-commit` schema validation (`npm run validate`) passes against the updated file
- [ ] The `_doc` comment in `config.json` is updated to name the board explicitly

## Test Plan

No automated tests required — change is non-functional (data file + schema only).
Manual: run `npm run validate` (or equivalent schema-validation command) to confirm the
updated `config.json` is accepted and that a config file with `hardware` omitted or set
to an unknown string is rejected.

## Documentation

- `data/config.json` — update `_doc` string to name the board: `"Hardware pin configuration for ESP32 (NodeMCU-32S). hardware field must match the compiled firmware target."`
- `docs/builders/` — if a key-reference doc exists, add `hardware` to the field table

## Notes

- `"additionalProperties": false` is already set in the schema, so no extra guard needed
- Pin range constraints (`minimum: 0, maximum: 39`) remain on individual pin fields;
  board-specific range validation is deferred to TASK-181 (web tool) and TASK-183 (app)
- This task is a prerequisite for TASK-180, TASK-181, TASK-182, and TASK-183
