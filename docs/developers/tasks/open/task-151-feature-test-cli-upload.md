---
id: TASK-151
title: Feature Test — CLI upload command
status: open
opened: 2026-04-19
effort: Small (2-4h)
complexity: Low
human-in-loop: Main
group: feature_test
---

## Description

Execute the functional tests for the `upload` subcommand of `scripts/pedal_config.py`
as defined in `docs/developers/FEATURE_TEST_PLAN.md` Part 1.3.

Requires a connected ESP32 pedal and a text editor on the host to verify keystrokes.

## Pre-conditions

- Python virtual environment with `pip install -r requirements.txt`
- ESP32 pedal flashed with current firmware (`make run-esp32`)
- Bluetooth-capable host; pedal powered on and in range
- Text editor open on host to receive keystrokes during U-02/U-03/U-04 variants
- `data/profiles.json` backed up (restore from git after U-06/U-07)

## Tests to execute

| Test ID | Description |
|---------|-------------|
| U-01 | Upload valid profiles.json → "OK" from pedal; LED blinks 3× |
| U-02 | After U-01: press buttons A–D → each fires the correct action |
| U-03 | Edit one profile's button value; upload; verify new keypress |
| U-04 | Upload schema-invalid profiles.json → CLI errors before connecting |
| U-05 | Disconnect pedal power mid-transfer → CLI reports connection loss clearly |
| U-06 | Upload `{"profiles": []}` → firmware error; CLI displays it |
| U-07 | Upload profiles with all actions set to `(none)` → upload succeeds; no error LED |

## Acceptance Criteria

- [ ] All U-01–U-07 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] Any failures filed as child tasks using the defect template
