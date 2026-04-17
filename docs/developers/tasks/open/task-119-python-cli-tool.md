---
id: TASK-119
title: Python CLI Tool for Profile Upload
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: BLEConfig
order: 5
prerequisites: [TASK-118, TASK-102]
---

## Description

Create `scripts/pedal_config.py` — a cross-platform Python CLI tool for scanning, uploading, and validating pedal configurations over BLE. Uses `bleak` for BLE communication and `jsonschema` for offline schema validation.

## Acceptance Criteria

- [ ] `scripts/pedal_config.py` created with subcommands: `scan`, `upload`, `upload-config`, `validate`
- [ ] `scan` — lists nearby BLE devices advertising the pedal service UUID (name, address, RSSI)
- [ ] `upload profiles.json` — sends `profiles.json` to pedal via `CONFIG_WRITE` characteristic (chunked, 512-byte MTU, 2-byte big-endian sequence number prefix); waits for `CONFIG_STATUS` `OK` or `ERROR:<msg>`
- [ ] `upload-config config.json` — same protocol but targets `CONFIG_WRITE_HW` characteristic
- [ ] `validate profiles.json` — validates against `data/profiles.schema.json` without connecting to the pedal; prints schema errors in human-readable form
- [ ] `scripts/requirements.txt` created with: `bleak`, `jsonschema`
- [ ] `scripts/README.md` updated with usage examples
- [ ] Exit codes: `0` = success, `1` = validation/connection error

## Files to Touch

- `scripts/pedal_config.py` (new)
- `scripts/requirements.txt` (new)
- `scripts/README.md` (updated)

## Test Plan

**Unit tests** (`scripts/tests/test_pedal_config.py`): covered by TASK-120.
Manual smoke test: `python scripts/pedal_config.py validate data/profiles.json` must pass.

## Prerequisites

- **TASK-118** — BLE service must be live on firmware before `upload` can be tested end-to-end
- **TASK-102** — `getJsonProperties` on `PinAction` ensures action configs can be round-tripped to JSON for export

## Notes

Use `asyncio` + `bleak` for BLE operations. Chunking logic: split payload into 510-byte chunks, prepend 2-byte big-endian sequence number starting at `0x0000`; send sentinel chunk with sequence `0xFFFF` and empty payload to signal end of transfer (or as defined in TASK-117 spec). The `validate` subcommand must work offline with no BLE hardware.
