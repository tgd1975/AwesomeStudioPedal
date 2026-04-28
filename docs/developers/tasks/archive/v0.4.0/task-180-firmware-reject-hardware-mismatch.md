---
id: TASK-180
title: Firmware — reject config with wrong hardware field at boot
status: closed
opened: 2026-04-20
effort: Medium (2-8h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
epic: hardware-selector
order: 2
prerequisites: [TASK-179]
---

## Description

Once `config.json` carries a `"hardware"` field (TASK-179), the firmware must read it
at boot and refuse to activate if the value does not match the compiled target.

Without this guard, an operator could accidentally flash an nRF52840 config onto an
ESP32 board (or vice versa). The pin numbers differ between the two boards, so silently
accepting a mismatched config could drive incorrect GPIOs — potentially damaging
connected hardware or producing silent misbehaviour.

Each board's `builder_config.h` already knows its target identity; expose that as a
compile-time string constant and compare it against the JSON field in the config loader.

Additionally, the device must expose its board identity over BLE so that the CLI
(TASK-182) and the Flutter app (TASK-183) can perform pre-upload validation without
requiring a separate out-of-band channel.

## Acceptance Criteria

- [ ] Each `builder_config.h` (ESP32 and nRF52840) defines a `CONF_HARDWARE_TARGET`
  string constant (`"esp32"` / `"nrf52840"`)
- [ ] Config loader reads `"hardware"` from `config.json`; if missing or not equal to
  `CONF_HARDWARE_TARGET`, it logs a clear error and halts activation (no profiles loaded,
  no BLE keyboard active) — the power LED blinks an error pattern instead of steady-on
- [ ] A new BLE Device Info characteristic (or an extension to the existing config
  service) exposes `CONF_HARDWARE_TARGET` as a readable string so clients can query it
- [ ] Host-test suite covers the mismatch path in the config loader
- [ ] On-device smoke test confirms the error blink pattern when a mismatched config is
  loaded onto the device

## Test Plan

**Host tests** (`make test-host`):

- Add `test/unit/test_hardware_mismatch.cpp`
- Cover: matching hardware field → normal activation; mismatched field → error state;
  missing field → error state

**On-device tests** (manual, requires physical hardware):

- Flash firmware, load a config with the wrong `hardware` value via CLI
- Verify the power LED blinks the error pattern and no keyboard events fire
- Requires: ESP32 or nRF52840 connected via USB

## Prerequisites

- **TASK-179** — adds the `hardware` field to `config.json` and schema; firmware reads this field

## Notes

- Error blink pattern: suggest 3× rapid blinks repeated (distinct from BLE-searching
  pattern); exact pattern is implementation detail, but must be documented in code
- The BLE characteristic for hardware identity can reuse the existing Config GATT service
  (see `docs/developers/BLE_CONFIG_GATT_SPEC.md`) — add a new read-only characteristic
  rather than a new service
- Human-in-loop is `Support` because on-device verification requires physical hardware
