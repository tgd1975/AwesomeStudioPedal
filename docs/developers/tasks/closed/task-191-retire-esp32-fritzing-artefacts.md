---
id: TASK-191
title: Retire ESP32 Fritzing artefacts from docs/media/
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 6
prerequisites: [TASK-190]
---

## Description

Remove the ESP32 Fritzing source and its exported PNG artefacts from `docs/media/`:
`AwesomeStudioPedal_esp32_wiring.fzz`, the breadboard PNG, the circuit PNG, and the PCB
PNG. Update any remaining links in the documentation that still point to these files.

This completes the ESP32 pilot: the WireViz SVG (committed in TASK-189 and embedded in
TASK-190) is now the canonical wiring reference, so the Fritzing files are no longer needed.

## Acceptance Criteria

- [ ] `docs/media/AwesomeStudioPedal_esp32_wiring.fzz` removed from the repo
- [ ] Fritzing-exported `*_breadboard.png`, `*_circuit.png`, `*_pcb.png` for ESP32 removed
- [ ] No remaining references to the deleted files in any Markdown doc
- [ ] `docs/builders/BUILD_GUIDE.md` and any other affected pages render without broken links

## Test Plan

No automated tests required — documentation cleanup only.

## Prerequisites

- **TASK-190** — WireViz SVG embedded in `BUILD_GUIDE.md`; the Fritzing PNG is no longer
  the active reference and can be safely deleted.

## Notes

- Search for all references before deleting: `grep -r "esp32_wiring" docs/` and
  `grep -r "_breadboard\|_circuit\|_pcb" docs/`.
- If the breadboard PNG is referenced as a *supplemental* illustration (not the schematic),
  evaluate whether it still has value. The simplified solution keeps it only if it is not
  replaced by the WireViz output — raise this for clarification if unclear.
- The proof-of-concept files in `docs/developers/ideas/open/` (idea-019-esp32-wiring.py,
  idea-019-esp32-schematic.svg) are separate from the Fritzing artefacts — do not remove
  them in this task; they are cleaned up as part of TASK-197.
