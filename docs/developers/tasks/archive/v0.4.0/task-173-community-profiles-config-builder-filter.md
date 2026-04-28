---
id: TASK-173
title: Web Config Builder — button-count filter in profiles gallery
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: CommunityProfiles
order: 8
---

## Description

Add a button-count filter control to the community profiles gallery modal introduced in
TASK-172. Filters the card list client-side from the already-fetched index — no
additional network requests.

## Acceptance Criteria

- [ ] Filter control (dropdown or button group) rendered inside the gallery modal
- [ ] Selecting N hides cards where `minButtons > N`
- [ ] A "Show all" / "Any" option is available and is the default
- [ ] When the modal is opened while a profile is already loaded in the builder, the filter defaults to that profile's button count
- [ ] Filter state resets to "Show all" when the modal is closed and reopened without an active profile

## Notes

Depends on: TASK-172.
