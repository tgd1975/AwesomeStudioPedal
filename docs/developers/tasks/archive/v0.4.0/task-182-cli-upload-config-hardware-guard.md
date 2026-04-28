---
id: TASK-182
title: CLI upload-config — validate hardware field before upload
status: closed
opened: 2026-04-20
effort: Small (<2h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Support
epic: hardware-selector
order: 4
prerequisites: [TASK-179, TASK-180]
---

## Description

The CLI `upload-config` command currently uploads `config.json` to the connected device
without checking whether the config targets the correct board. If an operator uploads a
config intended for nRF52840 to an ESP32 device (or vice versa), the firmware will detect
the mismatch at boot (TASK-180) and halt — but this is a poor experience that could
confuse users and leaves the device in a broken state until a correct config is re-uploaded.

Add a pre-upload hardware check to the CLI:

1. Read the `"hardware"` field from the config file being uploaded.
2. Query the connected device's hardware identity via the BLE characteristic added in
   TASK-180.
3. If they don't match, print a clear error and abort without writing any data to the
   device.

## Acceptance Criteria

- [ ] `upload-config` reads `config.json`'s `"hardware"` field; aborts with an error
  message if the field is absent
- [ ] `upload-config` queries the device's hardware identity BLE characteristic before
  writing
- [ ] If config `hardware` ≠ device identity, the upload is aborted with a message like:
  `Error: config targets nrf52840 but connected device is esp32. Upload aborted.`
- [ ] If config and device match, upload proceeds as before
- [ ] Unit tests cover: match → proceed, mismatch → abort, missing field → abort

## Test Plan

**Host tests** (`make test-host` / Python unit tests):

- Mock the BLE device identity response
- Cover: matching hardware → upload proceeds; mismatched hardware → upload aborted with
  correct error message; missing `hardware` field in config → aborted

**On-device tests** (manual, requires physical hardware):

- Attempt to upload a mismatched config via CLI; confirm abort message and device remains
  functional
- Requires: ESP32 or nRF52840 connected via USB/BLE

## Prerequisites

- **TASK-179** — adds the `hardware` field to `config.json` that the CLI reads
- **TASK-180** — firmware exposes the device's hardware identity over BLE for the CLI to query

## Notes

- Human-in-loop is `Support` for the on-device verification step
- The BLE characteristic to query is defined in TASK-180; coordinate on the characteristic
  UUID so the CLI and firmware agree
- If the device is too old to expose the hardware characteristic (e.g. firmware predates
  TASK-180), treat a missing/empty response as a warning rather than an error — print a
  warning and ask the user to confirm before proceeding
