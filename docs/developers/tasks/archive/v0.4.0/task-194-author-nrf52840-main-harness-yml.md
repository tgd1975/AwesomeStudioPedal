---
id: TASK-194
title: Author docs/builders/wiring/nrf52840/main-harness.yml
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Clarification
epic: wiring-as-code
order: 8
prerequisites: [TASK-193]
---

## Description

Create the WireViz YAML source file for the nRF52840 default build at
`docs/builders/wiring/nrf52840/main-harness.yml`. The harness must match the default
GPIO assignments in `src/nrf52840/builder_config.h` — buttons, select LEDs, power LED,
and Bluetooth LED. Follow the same structure and conventions established in the ESP32
harness (TASK-188).

## Acceptance Criteria

- [ ] `docs/builders/wiring/nrf52840/main-harness.yml` created and valid WireViz syntax
- [ ] All default GPIO assignments from the nRF52840 `builder_config.h` are represented
- [ ] `wireviz docs/builders/wiring/nrf52840/main-harness.yml` runs without errors

## Test Plan

No automated tests required — documentation source file only.

## Prerequisites

- **TASK-193** — CI staleness guard in place; the new nRF52840 harness will be covered
  by the guard automatically once committed.

## Notes

- Mirror the structure of `docs/builders/wiring/esp32/main-harness.yml` (TASK-188) for
  consistency — same section ordering, same comment style.
- Verify GPIO assignments from `src/nrf52840/builder_config.h` before writing; do not
  copy the ESP32 pin numbers.
- nRF52840 uses different voltage levels (3.3 V); LED resistor values may differ.
  Check `builder_config.h` or ask for clarification if resistor values are not documented.
