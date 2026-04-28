---
id: TASK-170
title: CI — minButtons consistency check for profiles/
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: CommunityProfiles
order: 5
---

## Description

Add a CI check that parses each profile set file under `profiles/`, counts the unique
button keys defined across all profiles in the set, and asserts that no key exceeds the
subfolder's declared minimum. For example: a file in `2-button/` must not define buttons
C or D.

Can be implemented as a small Node or Python script called from the same workflow as
TASK-169.

## Acceptance Criteria

- [ ] Script or CI step checks button count consistency for every file under `profiles/`
- [ ] Derives the allowed button count from the subfolder name (`N-button/` → max N buttons)
- [ ] CI fails with a clear message naming the offending file and button key if a violation is found
- [ ] Runs as part of the same workflow as TASK-169 (not a separate workflow trigger)
- [ ] Passes on all 12 starter profiles from TASK-167

## Notes

Button keys are the top-level keys of each profile's `buttons` object (e.g. `A`, `B`,
`C`, `D`). The check applies across all profiles within a set — if any profile in a
`2-button/` file uses key C, it fails.

Depends on: TASK-168 (folder structure and profiles exist), TASK-169 (workflow exists to extend).
