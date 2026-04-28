---
id: TASK-189
title: Generate ESP32 SVG + BOM and commit both
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 3
prerequisites: [TASK-188]
---

## Description

Run `wireviz` on `docs/builders/wiring/esp32/main-harness.yml` to produce
`main-harness.svg` and `main-harness.bom.tsv`. Verify that the SVG renders correctly
in GitHub's Markdown preview (open the file on github.com after pushing). Commit both
generated files alongside the YAML source.

Generated files are committed to the repo so that GitHub renders the SVG in Markdown
without requiring a build step — this is intentional per the simplified solution design.

## Acceptance Criteria

- [ ] `docs/builders/wiring/esp32/main-harness.svg` exists and is valid SVG
- [ ] `docs/builders/wiring/esp32/main-harness.bom.tsv` exists and contains component rows
- [ ] SVG renders correctly in GitHub's web UI (no broken image, all labels visible)
- [ ] Both files are committed to the repo alongside the YAML source

## Test Plan

No automated tests required — generated documentation assets only.

## Prerequisites

- **TASK-188** — `main-harness.yml` authored and valid; `wireviz` can process it without errors.

## Notes

- Run: `wireviz docs/builders/wiring/esp32/main-harness.yml`
- WireViz outputs files next to the YAML by default — verify output paths match the layout.
- The SVG should be checked on GitHub after the first push to confirm rendering; some SVG
  features are not supported by GitHub's inline renderer.
