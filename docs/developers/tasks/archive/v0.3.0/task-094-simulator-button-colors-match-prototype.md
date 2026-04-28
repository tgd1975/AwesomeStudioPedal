---
id: TASK-094
title: Match simulator button colors to physical prototype
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
---

## Description

The simulator's pedal buttons are currently all rendered in the same neutral blue
(`--btn-bg: #1e3a5f`). The physical prototype has distinctly colored button caps
(visible in `docs/media/prototype.jpg`):

| Slot | Color      | Suggested CSS value |
|------|------------|---------------------|
| A    | Orange     | `#f97316`           |
| B    | Red        | `#dc2626`           |
| C    | Hot pink   | `#ec4899`           |
| D    | Purple     | `#7c3aed`           |

Update the simulator so each button renders in its matching color.
For layouts with more than 4 buttons (E, F, …) a neutral fallback color is fine.

The color should apply to the button border and/or background in its idle state.
Held/active and delay-active states can keep their existing highlight styles.

## Acceptance Criteria

- [ ] Buttons A–D in the simulator match the prototype colors (orange, red, hot pink, purple)
- [ ] Buttons beyond D fall back gracefully (no broken styling)
- [ ] Held and delay-active states are still visually distinct from the idle color

## Notes

Reference image: `docs/media/prototype.jpg`
