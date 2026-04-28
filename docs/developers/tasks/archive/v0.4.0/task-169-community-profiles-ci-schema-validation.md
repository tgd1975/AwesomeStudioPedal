---
id: TASK-169
title: CI — schema validation for profiles/ PRs
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: CommunityProfiles
order: 4
---

## Description

Add a GitHub Actions CI step that triggers on any PR touching `profiles/**`. The step
runs `npm run validate-profiles` (TASK-168) and fails the PR if any profile file is
invalid against `data/profiles.schema.json`.

## Acceptance Criteria

- [ ] GitHub Actions workflow (new file or extended existing one) triggers on `pull_request` events when files under `profiles/**` are changed
- [ ] Step runs `npm run validate-profiles`
- [ ] CI fails with a clear error message if any profile file fails schema validation
- [ ] CI passes on the 12 starter profiles from TASK-167
- [ ] Step is skipped gracefully when no `profiles/` files are changed (use path filters)

## Notes

Check existing workflows under `.github/workflows/` before creating a new one — it may
be appropriate to add this as a job to an existing validation workflow.

Depends on: TASK-168.
