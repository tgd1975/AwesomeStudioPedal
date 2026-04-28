---
id: TASK-202
title: Extend schematic script for nRF52840 and generate SVG
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (1-2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: None
epic: circuit_diagram_as_code
order: 3
prerequisites: [TASK-201]
---

## Description

Extend `scripts/generate-schematic.py` to support `--target nrf52840` and generate the
nRF52840 circuit schematic SVG. The circuit topology is identical to the ESP32 build —
same component types, same resistor values — with different GPIO pin assignments and a
different MCU label.

GPIO assignments (source of truth: `lib/hardware/nrf52840/include/builder_config.h`):

| Signal | Pin | Direction |
|--------|-----|-----------|
| LED: Bluetooth | D5 | Output |
| LED: Power | D6 | Output |
| LED: Select 1/2/3 | D9, D10, D11 | Output |
| Button: SELECT | D12 | Input (pull-up) |
| Button: A/B/C/D | A0, A1, A2, A3 | Input (pull-up) |

## Acceptance Criteria

- [ ] `python scripts/generate-schematic.py --target nrf52840` runs without error
- [ ] `docs/builders/wiring/nrf52840/main-circuit.svg` generated and committed
- [ ] Pin labels in SVG match `lib/hardware/nrf52840/include/builder_config.h`
- [ ] Diagram style is consistent with the ESP32 diagram from TASK-201

## Test Plan

```bash
python scripts/generate-schematic.py --target nrf52840
# open docs/builders/wiring/nrf52840/main-circuit.svg in browser
# verify pin labels match builder_config.h
```

## Documentation

- `scripts/generate-schematic.py` — extended with nRF52840 support
- `docs/builders/wiring/nrf52840/main-circuit.svg` — generated artefact, committed
