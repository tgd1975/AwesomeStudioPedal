---
id: TASK-188
title: Author docs/builders/wiring/esp32/main-harness.yml
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Clarification
epic: wiring-as-code
order: 2
prerequisites: [TASK-187]
---

## Description

Create the WireViz YAML source file for the ESP32 default build at
`docs/builders/wiring/esp32/main-harness.yml`. The harness must match the default GPIO
assignments in `src/esp32/builder_config.h` — buttons, select LEDs, power LED, and
Bluetooth LED. Include connector types, wire gauges, and color codes where applicable.

This YAML is the human-editable source of truth for the ESP32 reference wiring diagram.
It is a *reference example* for the default config — not a mirror of every possible
builder customisation. The rendered pages will carry a "reference wiring" banner.

## Acceptance Criteria

- [ ] `docs/builders/wiring/esp32/main-harness.yml` created and valid WireViz syntax
- [ ] All default GPIO assignments from `builder_config.h` are represented (buttons A–D,
      SELECT, LEDs: power, BT, profile-select 1–3)
- [ ] `wireviz docs/builders/wiring/esp32/main-harness.yml` runs without errors

## Test Plan

No automated tests required — documentation source file only.

## Prerequisites

- **TASK-187** — WireViz and Graphviz installed and verified; `wireviz` CLI available.

## Notes

- GPIO assignments to use (default ESP32 config): BTN_A=13, BTN_B=12, BTN_C=27, BTN_D=14,
  BTN_SELECT=21, LED_PWR=25, LED_BT=26, LED_SEL1=5, LED_SEL2=18, LED_SEL3=19.
  Verify against the actual `builder_config.h` before writing.
- The file should be placed under `docs/builders/wiring/esp32/` (create the directory).
- Wire color convention: use standard hobbyist colors (red=VCC, black=GND, etc.).
- Ask for clarification if the connector type (pin headers, screw terminals, etc.) is not
  documented anywhere — this affects the WireViz `connectors:` section.
