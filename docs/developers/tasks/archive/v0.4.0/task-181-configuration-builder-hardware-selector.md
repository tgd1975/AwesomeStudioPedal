---
id: TASK-181
title: Configuration Builder — hardware selector, per-board defaults, and pinout diagram
status: closed
opened: 2026-04-20
effort: Medium (2-8h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: hardware-selector
order: 3
prerequisites: [TASK-179]
---

## Description

The `docs/tools/configuration-builder/index.html` tool currently assumes ESP32 hardware
throughout: pin ranges are hard-coded to 0–39, defaults match the ESP32 `builder_config.h`,
and there is no way to target an nRF52840 build.

This task extends the tool with three linked capabilities:

1. **Hardware selector** — a `<select>` at the top of the form with two options:
   `ESP32 (NodeMCU-32S)` and `nRF52840 (Adafruit Feather)`. Switching the selector
   pre-fills all pin fields with the correct defaults for that board and updates
   validation ranges.

2. **Pinout diagram** — an SVG pinout of the selected board rendered alongside the pin
   assignment table. Pins that are assigned in the current config are highlighted on the
   diagram in real time as the user edits the form.

3. **`hardware` field in JSON output** — the exported `config.json` includes
   `"hardware": "<board>"` matching the selector value, satisfying the schema added in
   TASK-179.

## Acceptance Criteria

- [ ] A board selector `<select>` appears at the top of the form; options are
  `ESP32 (NodeMCU-32S)` and `nRF52840 (Adafruit Feather nRF52840 Express)`
- [ ] Selecting a board pre-fills all pin inputs with the defaults from the corresponding
  `builder_config.h` and updates per-pin `min`/`max` constraints
- [ ] The JSON preview and exported file always include `"hardware": "esp32"` or
  `"hardware": "nrf52840"` matching the selector
- [ ] An SVG pinout diagram for each board is shown; currently-assigned pin numbers are
  highlighted on the diagram and update live as the user types
- [ ] Loading an existing `config.json` that contains a `"hardware"` field automatically
  sets the selector to the correct board
- [ ] A pin number that is invalid for the selected board (e.g. pin 34 on nRF52840 Feather)
  triggers a visible inline warning without blocking export

## Test Plan

No automated tests required — change is a browser-side HTML/JS tool with no compiled
logic. Manual verification:

- Open in Chromium and Firefox at 1280 px and 375 px widths
- Switch between boards; confirm defaults change and diagram updates
- Export both board configs; validate exported JSON against `config.schema.json`
- Load a saved nRF52840 config; confirm selector snaps to nRF52840 automatically

## Documentation

- `docs/builders/` — update Configuration Builder usage docs to describe the hardware
  selector and pinout diagram

## Prerequisites

- **TASK-179** — adds `"hardware"` to the schema; this task emits that field in the JSON output

## Notes

- nRF52840 Feather valid GPIO pins: D5, D6, D9, D10, D11, D12 (digital); A0–A5 (14–19).
  Pins outside this set should show a warning, not a hard block.
- SVG pinouts: create minimal labelled diagrams (not photo-realistic); include pin labels
  matching the physical board silkscreen. Static SVG assets are acceptable — no external
  diagram library needed.
- This task is CSS/HTML/JS only — no changes to firmware or app code
- TASK-163 (design implementation) changes the visual style of this tool; coordinate so
  the selector and diagram follow the same design tokens. These tasks can be done in
  either order but the final result must be consistent.
