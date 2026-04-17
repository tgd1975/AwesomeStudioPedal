---
id: TASK-074
title: Update developers documentation - add cross-reference to wiring diagram
status: closed
closed: 2026-04-16
opened: 2026-04-11
effort: Small (<2h)
complexity: Junior
human-in-loop: No
---

## Description

Add a cross-reference to the wiring diagram files in the developer documentation. The wiring diagrams were added as part of the circuit/prototype work and are not yet referenced from any developer doc.

## Acceptance Criteria

- [ ] Add a "Hardware wiring" section or note in `docs/developers/ARCHITECTURE.md` linking to the diagrams
- [ ] Link to the PNG circuit image for quick viewing: `../media/AwesomeStudioPedal_esp32_wiring_circuit.png`
- [ ] Link to the Fritzing source file for editing: `../media/AwesomeStudioPedal_esp32_wiring.fzz`
- [ ] Optionally add a one-line reference in `DEVELOPMENT_SETUP.md` pointing to ARCHITECTURE.md

## Available wiring files (in `docs/media/`)

- `AwesomeStudioPedal_esp32_wiring_circuit.png` — schematic, best for quick reference
- `AwesomeStudioPedal_esp32_wiring_breadboard.png` — breadboard layout
- `AwesomeStudioPedal_esp32_wiring_pcb.png` — PCB view
- `AwesomeStudioPedal_esp32_wiring.fzz` — Fritzing source

## Notes

- ARCHITECTURE.md already has a "High-level component diagram" section — place the wiring reference there or directly after it
- Do not duplicate the Mermaid pin diagram that already lives in BUILD_GUIDE.md
