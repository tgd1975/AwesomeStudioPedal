---
id: TASK-176
title: Flutter — Community Profiles screen
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: CommunityProfiles
order: 11
---

## Description

Create a "Community Profiles" screen in the Flutter app. Sources data from
`CommunityProfilesService` (TASK-175). Shows a scrollable card list with search and
filter controls. Tapping a card previews the profile set; a "Load" button routes to the
existing profile editor.

## Acceptance Criteria

- [ ] Screen renders index cards; each card shows: name, description, tags, minButtons badge, profileCount, author
- [ ] Cards with `minFirmware` set show a warning (e.g. "Requires firmware ≥ X.Y.Z")
- [ ] Search field filters cards by name and tag, client-side, no re-fetch
- [ ] Button-count dropdown filters by `minButtons`
- [ ] Tapping a card expands or opens a preview panel showing: profile names in the set, button action names from the first profile
- [ ] "Load" button calls `fetchProfileSet(id)` and navigates to the existing profile editor (same code path as loading from file)
- [ ] Loading state shown while fetching index; error state shown with retry option on failure
- [ ] Widget tests cover: card list render, search filter, button-count filter, load action

## Notes

Depends on: TASK-175.
