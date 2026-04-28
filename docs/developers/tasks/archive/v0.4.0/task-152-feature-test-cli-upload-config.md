---
id: TASK-152
title: Feature Test — CLI upload-config command
status: closed
closed: 2026-04-23
opened: 2026-04-19
effort: Small (1-2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 14
prerequisites: [TASK-235]
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

- [x] All C-01–C-03 tests executed and results recorded in FEATURE_TEST_PLAN.md.
- [x] Failures filed as defect tasks.

## Results summary

- **C-01 ✓**, **C-02 ✓** — upload happy-path and post-reboot persistence both pass.
- **C-03 ✗** — both CLI and schema fail to catch a `numButtons` / `buttonPins`
  length mismatch; firmware also accepts the inconsistent config. Tracked in
  TASK-233 (scope expanded to cover cross-field schema constraints in
  `config.schema.json`).

## Defects uncovered in the process

- **TASK-235** — hardware-identity BLE characteristic was returning 4 bytes
  of the `const char*` pointer (NimBLE's templated `setValue(const T&)` takes
  `sizeof(T)` for non-string types). `upload-config`'s identity guard was
  therefore tripping with `ERROR: hardware mismatch: config targets 'esp32'
  but connected device is '\xff\xff@?'` on every call. Fixed by wrapping in
  `std::string`.

## Notes

- C-02 surfaced a **BlueZ auto-reconnect gap**: after a pedal reboot the
  host BT daemon did not re-establish the HID bond automatically; a manual
  `bluetoothctl connect <addr>` was needed. Noted in FEATURE_TEST_PLAN.md
  for future triage.
- C-02 also surfaced a **usability learning**: Profile 01 "Score Navigator"
  maps to cursor-motion keys (PageUp/PageDown/Home/End) that have no visible
  effect in a short document; operators can misread "no visible effect" as
  "nothing happened." Noted in FEATURE_TEST_PLAN.md.
