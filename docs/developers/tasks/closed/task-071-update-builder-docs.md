---
id: TASK-071
title: Update builder documentation - add Fritzing wiring diagrams and prototype pictures
status: closed
closed: 2026-04-16
opened: 2026-04-11
effort: Medium (2-8h)
complexity: Junior
human-in-loop: No
---

## Description

Update `docs/builders/BUILD_GUIDE.md` to include the Fritzing wiring diagrams and prototype pictures. The file already has a Mermaid wiring diagram — the Fritzing images should be added as visual complements directly after that section, not as replacements.

## Acceptance Criteria

- [ ] Add the three Fritzing PNG exports after the existing Mermaid diagram in BUILD_GUIDE.md
- [ ] Add a prototype pictures gallery section at the end of BUILD_GUIDE.md
- [ ] All image paths are relative (files are in `docs/media/`, prefix `../media/`)
- [ ] No changes to the existing Mermaid diagram

## Available files (all in `docs/media/`)

- `AwesomeStudioPedal_esp32_wiring_breadboard.png` — breadboard view
- `AwesomeStudioPedal_esp32_wiring_circuit.png` — circuit schematic view
- `AwesomeStudioPedal_esp32_wiring_pcb.png` — PCB view
- `AwesomeStudioPedal_esp32_wiring.fzz` — Fritzing source file (link only, don't embed)

## Prototype pictures (in `docs/media/`, path prefix `../media/`)

- `prototype.jpg` — overview shot
- `prototype_esp32_front.jpg`, `prototype_esp32_side.jpg`, `prototype_esp32_top.jpg` — ESP32 detail
- `prototype_pedal_open.jpg`, `prototype_pedal_pressed.jpg` — pedal in use
- `prototype_wiring.jpg` — wiring detail
