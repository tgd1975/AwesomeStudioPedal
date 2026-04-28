---
id: TASK-150
title: Feature Test — CLI scan command
status: closed
closed: 2026-04-23
opened: 2026-04-19
effort: Small (1-2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 12
prerequisites: [TASK-227]
---

## Description

Execute the functional and usability tests for the `scan` subcommand of
`scripts/pedal_config.py` as defined in `docs/developers/FEATURE_TEST_PLAN.md`
Part 1.2 and usability item CU-03.

## Pre-conditions

- Python virtual environment with `pip install -r requirements.txt`
- Bluetooth-capable host machine
- For S-01: ESP32 pedal powered on and within BLE range
- For S-04: two pedals powered on simultaneously (optional, skip if only one pedal available)

## Tests to execute

| Test ID | Description |
|---------|-------------|
| S-01 | Pedal on → appears in scan output within 5 s with name and RSSI |
| S-02 | Pedal off → output says no devices found; no crash |
| S-03 | BLE adapter disabled on host → clear error message; no traceback |
| S-04 | Two pedals on → both appear with distinct MAC addresses |
| CU-03 | Usability: scan with no pedal — output must not look like a crash |

## Acceptance Criteria

- [ ] S-01 through S-04 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] CU-03 usability finding documented
- [ ] Any failures filed as child tasks using the defect template
