---
id: TASK-152
title: Feature Test — CLI upload-config command
status: open
opened: 2026-04-19
effort: Small (1-2h)
complexity: Low
human-in-loop: Main
group: feature_test
---

## Description

Execute the functional tests for the `upload-config` subcommand of `scripts/pedal_config.py`
as defined in `docs/developers/FEATURE_TEST_PLAN.md` Part 1.4.

## Pre-conditions

- Python virtual environment with `pip install -r requirements.txt`
- ESP32 pedal flashed with current firmware (`make run-esp32`)
- `data/config.json` at repo root
- Bluetooth-capable host; pedal powered on and in range

## Tests to execute

| Test ID | Description |
|---------|-------------|
| C-01 | Upload config.json → "OK" from pedal; LED blinks 3× |
| C-02 | Reboot pedal after C-01 → hardware config takes effect; buttons work correctly |
| C-03 | Upload config.json with `numButtons: 5` but only 4 `buttonPins` → CLI catches mismatch before upload |

## Acceptance Criteria

- [ ] All C-01–C-03 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] Any failures filed as child tasks using the defect template
