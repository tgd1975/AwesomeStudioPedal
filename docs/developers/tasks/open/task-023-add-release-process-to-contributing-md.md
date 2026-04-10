---
id: TASK-023
title: Add release process to CONTRIBUTING.md
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Document the release process in `CONTRIBUTING.md` so maintainers know how to cut a release.
This includes tagging, triggering the release workflow, and post-release cleanup.

## Acceptance Criteria

- [ ] `CONTRIBUTING.md` has a "Release Process" section
- [ ] Section covers: creating a version tag, triggering the workflow, verifying the release
- [ ] Links to TASK-029 cleanup script and the release checklist (`docs/developers/RELEASE_CHECKLIST.md`)
- [ ] Follows Conventional Commits versioning conventions already documented in CONTRIBUTING.md

## Notes

Dependency: TASK-020 (release workflow) should be completed or in progress so the documented steps match reality.
