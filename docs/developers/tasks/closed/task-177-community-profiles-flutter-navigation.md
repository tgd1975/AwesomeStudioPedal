---
id: TASK-177
title: Flutter — wire Community Profiles into app navigation
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: CommunityProfiles
order: 12
---

## Description

Add "Community Profiles" as a source option in the app's profile-load flow, alongside
"Load from file" and "Load from device (BLE)". The option must be accessible without an
active BLE connection — it is a useful onboarding step before pairing.

## Acceptance Criteria

- [ ] "Community Profiles" option visible on the home or profile-load screen
- [ ] Tapping the option navigates to the Community Profiles screen (TASK-176)
- [ ] The option is accessible regardless of BLE connection state
- [ ] Existing "Load from file" and BLE upload flows are unaffected
- [ ] Navigation can be exercised in a widget test (mock the service)

## Notes

Depends on: TASK-176.
