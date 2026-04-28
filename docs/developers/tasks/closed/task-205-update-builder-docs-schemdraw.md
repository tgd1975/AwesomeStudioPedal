---
id: TASK-205
title: Update builder documentation — replace harness diagrams with circuit schematics
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: None
epic: circuit_diagram_as_code
order: 6
prerequisites: [TASK-201, TASK-202]
---

## Description

Update `docs/builders/BUILD_GUIDE.md` to reference the new Schemdraw-generated circuit
schematic SVGs and remove all references to WireViz harness SVGs, BOM TSV files, and
WireViz YAML sources.

The diagram type is changing: from a wiring-harness view (connectors, wire colours, cable
routing) to a circuit schematic (component values, GPIO nets, power and GND rails). Update
any surrounding explanatory text to match.

## Acceptance Criteria

- [ ] All `main-harness.svg` image references in `BUILD_GUIDE.md` replaced with
      `main-circuit.svg` references
- [ ] All `main-harness.bom.tsv` references removed
- [ ] All `main-harness.yml` references removed (source YAML files are being deleted)
- [ ] `wireviz` command reference replaced with
      `python scripts/generate-schematic.py --target esp32|nrf52840`
- [ ] Section headings or captions updated to say "circuit schematic" rather than
      "wiring harness" or "wiring diagram"
- [ ] No broken image links — all referenced SVGs exist and render in GitHub Markdown preview
- [ ] Any mention of WireViz in builder-facing prose removed

## Test Plan

Open `docs/builders/BUILD_GUIDE.md` in the GitHub Markdown preview and verify all diagrams
render. Confirm no references to `main-harness.*` or `wireviz` remain.

## Documentation

- `docs/builders/BUILD_GUIDE.md` — updated image references and prose
