---
id: TASK-050
title: Provide Fritzing wiring diagram
status: closed
closed: 2026-04-16
opened: 2026-04-10
effort: Medium (2-8h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Create a Fritzing wiring diagram showing how to connect all components (ESP32, buttons, LEDs,
power) and include it in the documentation to help builders assemble the pedal.

## Acceptance Criteria

- [x] Fritzing `.fzz` source file committed under `docs/media/`
- [x] Exported PNGs also committed for easy viewing (breadboard, circuit, PCB)
- [x] Diagram linked from builder documentation (TASK-026) and/or `README.md`
- [x] All connections labeled clearly

## Notes

Target audience: builders who are assembling the pedal from scratch and need a visual guide.
Fritzing is the most common tool for this type of diagram in the hobbyist/maker community.
