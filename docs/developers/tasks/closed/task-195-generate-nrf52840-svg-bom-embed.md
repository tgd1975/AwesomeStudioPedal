---
id: TASK-195
title: Generate nRF52840 SVG + BOM, commit, embed in docs, add banner
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 9
prerequisites: [TASK-194]
---

## Description

Run `wireviz` on `docs/builders/wiring/nrf52840/main-harness.yml` to produce
`main-harness.svg` and `main-harness.bom.tsv`. Commit both generated files. Then update
the nRF52840 build guide (or equivalent page in `docs/builders/`) to embed the SVG and
add the standard "reference wiring" banner — mirroring what was done for the ESP32 in
TASK-189 and TASK-190.

## Acceptance Criteria

- [ ] `docs/builders/wiring/nrf52840/main-harness.svg` exists and renders on GitHub
- [ ] `docs/builders/wiring/nrf52840/main-harness.bom.tsv` exists with component rows
- [ ] nRF52840 builder doc references the new SVG instead of any Fritzing PNG
- [ ] "Reference wiring" banner is present on the nRF52840 wiring page

## Test Plan

No automated tests required — generated documentation assets only.

## Documentation

- nRF52840 builder documentation page — replace Fritzing PNG reference with WireViz SVG
  and add the "reference wiring" banner.

## Prerequisites

- **TASK-194** — `main-harness.yml` for nRF52840 authored and valid.

## Notes

- Follow the same steps as TASK-189 + TASK-190 combined, applied to the nRF52840 target.
- Verify SVG rendering in GitHub's web UI after the first push.
