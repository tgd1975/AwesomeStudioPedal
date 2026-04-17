---
id: TASK-117
title: BLE Config GATT Service Spec Document
status: open
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: BLEConfig
order: 3
prerequisites: [TASK-115]
---

## Description

Write the BLE Config GATT service specification as a markdown document. This produces documentation only — no firmware change. The document is the authoritative source for UUIDs and protocol details consumed by TASK-118 (firmware), TASK-119 (CLI), and TASK-124 (Flutter app).

## Acceptance Criteria

- [ ] `docs/developers/BLE_CONFIG_PROTOCOL.md` created
- [ ] Document defines the project-specific 128-bit service UUID base
- [ ] `CONFIG_WRITE` characteristic: UUID, `WRITE_NO_RESPONSE`, chunked protocol (2-byte big-endian sequence number + up to 510 bytes payload per packet, 512-byte MTU)
- [ ] `CONFIG_STATUS` characteristic: UUID, `NOTIFY`, values: `OK`, `ERROR:<reason>`, `BUSY`
- [ ] Reassembly flow described: sequence numbers, end-of-transfer signal (e.g. sequence `0xFFFF`), buffer flush on error
- [ ] Hardware config upload: separate `CONFIG_WRITE_HW` characteristic (or same characteristic with a type prefix — decide and document)
- [ ] LED confirmation behaviour on success and failure documented (3× blink success, 1× long blink failure)
- [ ] Concurrent-upload rejection (`BUSY`) documented

## Files to Touch

- `docs/developers/BLE_CONFIG_PROTOCOL.md` (new)

## Test Plan

Review only — no code to test.

## Prerequisites

- **TASK-115** — schema format must be decided before the protocol doc can reference it for validation

## Notes

Suggested 128-bit UUID base: `12340000-0000-0000-0000-000000000000` (replace with a proper random UUID before shipping). Characteristics:

| Characteristic | UUID suffix | Properties |
|---|---|---|
| `CONFIG_WRITE` | `...0001` | WRITE_NO_RESPONSE |
| `CONFIG_STATUS` | `...0002` | NOTIFY |
| `CONFIG_WRITE_HW` | `...0003` | WRITE_NO_RESPONSE |

Use a real UUID generator (e.g. `uuidgen`) when writing the spec — do not use placeholder values in production code.
