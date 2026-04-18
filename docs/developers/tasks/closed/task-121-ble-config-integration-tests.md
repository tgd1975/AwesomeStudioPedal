---
id: TASK-121
title: BLE Config Integration Tests (Host + On-Device)
status: closed
opened: 2026-04-17
effort: Large (>8h)
complexity: Senior
human-in-loop: Support
group: BLEConfig
order: 7
prerequisites: [TASK-118]
---

## Description

End-to-end integration tests for the BLE config service: host-side tests (already covered in TASK-120) plus an on-device Unity test suite with a Python runner acting as the BLE client.

## Acceptance Criteria

### On-device Unity tests (`test/test_ble_config_esp32/`)
- [ ] `test/test_ble_config_esp32/` directory created with Unity test firmware
- [ ] `test/test_ble_config_esp32/runner.py` Python script acts as BLE client
- [ ] **Valid upload test:** `runner.py` connects, uploads known-good `profiles.json`, waits for `CONFIG_STATUS OK`; serial log confirms new profile is active
- [ ] **LED confirmation test:** select LEDs blink 3× after successful upload (verified via `digitalRead` on LED pins in test firmware)
- [ ] **Error recovery test:** `runner.py` sends malformed JSON → `ERROR:parse_failed`; then sends valid JSON → `OK` — service resets cleanly
- [ ] **Persistence test:** after successful upload, `runner.py` triggers soft reset, reconnects after 3 s, verifies new profile still active (loaded from LittleFS)
- [ ] `make test-esp32-ble-config` target added to `Makefile`
- [ ] Test documented as requiring: ESP32 (NodeMCU-32S) connected via USB + BLE-capable host (Linux with `bleak`, or macOS)

## Files to Touch

- `test/test_ble_config_esp32/` (new directory)
- `test/test_ble_config_esp32/test_main.cpp` (new)
- `test/test_ble_config_esp32/runner.py` (new)
- `Makefile` — add `test-esp32-ble-config` target

## Test Plan

**On-device** (`make test-esp32-ble-config`):
- Requires: ESP32 (NodeMCU-32S) connected via USB + BLE-capable host machine

## Prerequisites

- **TASK-118** — BLE config service must be live on firmware before these tests can run

## Notes

This is the only hardware-required task in Group D (on-device part). The host-side portion of BLE testing is in TASK-120. `runner.py` should reuse the chunking logic from `scripts/pedal_config.py` (TASK-119) — import it as a module rather than duplicating it.
