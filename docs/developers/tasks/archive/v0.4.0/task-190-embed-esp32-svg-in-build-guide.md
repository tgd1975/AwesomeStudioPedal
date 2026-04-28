---
id: TASK-190
title: Embed ESP32 wiring SVG in BUILD_GUIDE.md with reference banner
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 4
prerequisites: [TASK-189]
---

## Description

Replace the existing `![Circuit schematic]` Fritzing PNG reference in
`docs/builders/BUILD_GUIDE.md` with the newly generated WireViz SVG. Add an italicised
banner below the image making clear this is a reference diagram for the default config.

The banner text (from the simplified solution spec):
> *Reference wiring for the default config. Your build may differ.*

## Acceptance Criteria

- [ ] `BUILD_GUIDE.md` references `../wiring/esp32/main-harness.svg` instead of the
      Fritzing PNG
- [ ] Reference banner appears directly below (or above) the embedded image
- [ ] No broken image links remain for the replaced reference
- [ ] Markdown renders correctly in GitHub preview

## Test Plan

No automated tests required — documentation change only.

## Documentation

- `docs/builders/BUILD_GUIDE.md` — replace Fritzing PNG image reference with WireViz SVG
  and add the "reference wiring" banner.

## Prerequisites

- **TASK-189** — `main-harness.svg` generated and committed; path is stable.

## Notes

- Image reference syntax: `![ESP32 reference wiring](wiring/esp32/main-harness.svg)`
  (adjust relative path based on `BUILD_GUIDE.md` location).
- Keep the existing logical GPIO map (Mermaid) and breadboard illustration unchanged —
  only the schematic/circuit PNG is replaced in this task.
