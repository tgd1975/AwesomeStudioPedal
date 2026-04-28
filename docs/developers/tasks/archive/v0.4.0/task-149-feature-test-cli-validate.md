---
id: TASK-149
title: Feature Test — CLI validate command
status: closed
closed: 2026-04-21
opened: 2026-04-19
effort: Small (1-2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 11
---

## Description

Execute the functional and usability tests for the `validate` subcommand of
`scripts/pedal_config.py` as defined in `docs/developers/FEATURE_TEST_PLAN.md`
Part 1.1 and usability items CU-01/CU-02.

No hardware required for this task — all tests run on the host.

## Pre-conditions

- Python virtual environment with `pip install -r requirements.txt`
- `data/profiles.json` and `data/config.json` at the repo root (restore from git if modified)

## Tests to execute

| Test ID | Description |
|---------|-------------|
| V-01 | Validate valid profiles.json → exit 0 |
| V-02 | Validate config.json with `--hw` → exit 0 |
| V-03 | Validate profiles.json missing `buttons` field → exit 1, readable error |
| V-04 | Validate profiles.json with action missing `value` → exit 1, mentions field |
| V-05 | Validate a non-existent file → non-zero exit, no traceback |
| V-06 | Validate a truncated (invalid) JSON file → non-zero exit, no traceback |
| CU-01 | Usability: can a non-developer understand the error message from V-03? |
| CU-02 | Usability: can a non-developer fix a typo using the validate output alone? |

## Acceptance Criteria

- [ ] All V-01–V-06 tests pass (mark in FEATURE_TEST_PLAN.md)
- [ ] CU-01 and CU-02 usability findings documented (or rated as-is)
- [ ] Any failures filed as child tasks using the defect template in FEATURE_TEST_PLAN.md
