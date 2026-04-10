---
id: TASK-050
title: Provide Fritzing wiring diagram
status: open
effort: Medium (2-8h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Create a Fritzing wiring diagram showing how to connect all components (ESP32, buttons, LEDs,
power) and include it in the documentation to help builders assemble the pedal.

## Acceptance Criteria

- [ ] Fritzing `.fzz` source file committed under `docs/hardware/` or similar
- [ ] Exported PNG/SVG also committed for easy viewing
- [ ] Diagram linked from builder documentation (TASK-026) and/or `README.md`
- [ ] All connections labeled clearly

## Notes

Target audience: builders who are assembling the pedal from scratch and need a visual guide.
Fritzing is the most common tool for this type of diagram in the hobbyist/maker community.
