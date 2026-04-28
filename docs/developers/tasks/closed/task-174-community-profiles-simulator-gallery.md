---
id: TASK-174
title: Web Simulator — "Choose a starting point" gallery
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: CommunityProfiles
order: 9
---

## Description

Replace the hardcoded `example.json` auto-load in `docs/simulator/simulator.js` with a
"Choose a starting point" screen shown on page load. The screen presents community
profile cards (reusing the shared `profiles-gallery.js` module from TASK-172) and a
"Start blank" button. Selecting a card loads that profile set into the simulator.

## Acceptance Criteria

- [ ] Page load shows the gallery screen instead of auto-loading `example.json`
- [ ] Gallery fetch and card render reuse the shared `profiles-gallery.js` module from TASK-172 (no duplicated fetch/render logic)
- [ ] "Start blank" button loads an empty/default profile into the simulator
- [ ] Selecting a card loads that profile set into the simulator via the existing load path
- [ ] If the index fetch fails, the screen falls back to a "Start blank" prompt with an error note
- [ ] The old `example.json` is preserved in `docs/simulator/` (it is still referenced in builder docs and onboarding)

## Notes

Depends on: TASK-172 (shared gallery module), TASK-167 (profiles available on Pages for testing).
