---
id: TASK-196
title: Retire remaining Fritzing artefacts from docs/media/
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 10
prerequisites: [TASK-195]
---

## Description

After both ESP32 (TASK-191) and nRF52840 (TASK-195) wiring diagrams have been replaced by
WireViz SVGs, perform a final sweep of `docs/media/` to remove any Fritzing artefacts that
remain — `.fzz` files, breadboard PNGs, circuit PNGs, or PCB PNGs — for targets other than
ESP32. Update any remaining links in the documentation.

If TASK-191 has already cleaned up all Fritzing files, this task is a verification pass
confirming nothing was missed.

## Acceptance Criteria

- [ ] No `.fzz` files remain anywhere in the repo
- [ ] No Fritzing-exported PNGs (`*_breadboard.png`, `*_circuit.png`, `*_pcb.png`) remain
- [ ] `grep -r "\.fzz\|fritzing" docs/` returns no active references
- [ ] All doc pages render without broken image links

## Test Plan

No automated tests required — documentation cleanup only.

## Prerequisites

- **TASK-195** — nRF52840 WireViz SVG committed and embedded; both targets are now covered
  by WireViz, making the Fritzing fallback unnecessary.

## Notes

- Run `grep -r "\.fzz\|fritzing\|_breadboard\|_circuit\|_pcb" docs/` to find any
  lingering references before deleting files.
- If any Fritzing files remain in `docs/media/` that are not referenced anywhere, they can
  be deleted silently. If they are referenced, update the links first.
