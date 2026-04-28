---
id: TASK-197
title: Close IDEA-019 — mark wiring-as-code as implemented
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 11
prerequisites: [TASK-195]
---

## Description

Update `docs/developers/ideas/open/idea-019-wiring-as-code.md` (and its siblings) to
reflect that IDEA-019 has been implemented via the simplified WireViz solution. Move the
idea files to `docs/developers/ideas/closed/` or annotate them as implemented. Remove or
archive the proof-of-concept Schemdraw files (`idea-019-esp32-wiring.py`,
`idea-019-esp32-schematic.svg`) that are no longer needed.

## Acceptance Criteria

- [ ] `idea-019-wiring-as-code.md` annotated as implemented (or moved to `ideas/closed/`)
- [ ] `idea-019-simplified-solution.md` links to the implemented tasks
- [ ] Schemdraw proof-of-concept files removed from `docs/developers/ideas/open/`
- [ ] No dead links introduced by the cleanup

## Test Plan

No automated tests required — documentation/idea tracking change only.

## Prerequisites

- **TASK-195** — Both ESP32 and nRF52840 WireViz harnesses committed and embedded;
  the implementation is complete enough to close the idea.

## Notes

- Files to evaluate for removal/archival: `idea-019-esp32-wiring.py`,
  `idea-019-esp32-schematic.svg`, `idea-019-concept.md`, `idea-019-discarded-solution.md`.
- The simplified solution file (`idea-019-simplified-solution.md`) should be kept or linked
  as the implementation reference if the ideas folder uses a "closed with notes" convention.
- Check how other closed ideas are handled in `docs/developers/ideas/` before deciding
  the exact convention.

## Re-close note (TASK-207, 2026-04-22)

This task originally closed IDEA-019 as implemented with WireViz (TASK-187–199). However,
WireViz produced wiring-harness views, not the circuit schematics the idea called for. The
idea was reopened and properly implemented via TASK-200–207 using Schemdraw:

- `scripts/generate-schematic.py` — Python script drawing circuit schematics for both targets
- `docs/builders/wiring/<target>/main-circuit.svg` — generated and committed SVGs
- Pre-commit hook regenerates SVGs when source files change
- CI staleness guard fails if committed SVGs are out of date
- WireViz harness files and `requirements-wiring.txt` removed

Two prior approaches were evaluated and rejected: WireViz (wrong diagram type) and atopile
(requires KiCad, not a standalone SVG renderer). IDEA-019 is now correctly closed.
