---
id: TASK-201
title: Move and adapt ESP32 circuit schematic script
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (1-2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: Approval
epic: circuit_diagram_as_code
order: 2
prerequisites: [TASK-200]
---

## Description

Move the proof-of-concept Schemdraw script from the ideas archive to
`scripts/generate-schematic.py` and adapt it to be the canonical schematic generator
for both boards. Run it for the ESP32 build and commit the resulting SVG.

The existing script lives at `docs/developers/ideas/archive/idea-019-esp32-wiring.py`.
It already correctly draws:

- Power LED, BT LED, and 3 profile-select LEDs (GPIO outputs, 220 Ω resistors to GND)
- 4 action buttons + SELECT button (GPIO inputs, pull-up, active LOW)
- Reads GPIO assignments from `data/config.json`

Adapt it to:

- Accept a `--target esp32|nrf52840` argument
- Accept an `--output PATH` argument (default: `docs/builders/wiring/<target>/main-circuit.svg`)
- Keep the ESP32 config-file-driven path; add nRF52840 as a hardcoded pin map matching
  `lib/hardware/nrf52840/include/builder_config.h` (implemented in TASK-202)

## Acceptance Criteria

- [ ] `scripts/generate-schematic.py` committed with `--target esp32` support
- [ ] `python scripts/generate-schematic.py --target esp32` runs without error
- [ ] `docs/builders/wiring/esp32/main-circuit.svg` generated and committed
- [ ] SVG shows: ESP32 MCU label, labelled GPIO pins, 220 Ω resistors, LEDs, push buttons,
      GND rails — legible at normal screen resolution
- [ ] **Human sign-off**: reviewer confirms diagram quality is acceptable before TASK-202 proceeds
- [ ] Proof-of-concept files removed from ideas archive:
      `idea-019-esp32-wiring.py` and `idea-019-esp32-schematic.svg`

## Test Plan

```bash
python scripts/generate-schematic.py --target esp32
# open docs/builders/wiring/esp32/main-circuit.svg in browser
# verify GPIO labels match data/config.json
```

## Notes

The script already exists and works — this is mostly a move + minor adaptation, not a
rewrite. Keep the config-driven approach for ESP32 (reads `data/config.json`). Changing
a GPIO assignment in config.json must produce a correct updated SVG on next run.

## Documentation

- `scripts/generate-schematic.py` — moved and adapted from ideas archive
- `docs/builders/wiring/esp32/main-circuit.svg` — generated artefact, committed
