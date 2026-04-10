---
id: TASK-021
title: Add package publishing
status: open
effort: Large (8-24h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Evaluate whether firmware binaries should be published as packages (e.g. via PlatformIO registry,
GitHub Packages, or similar) and implement publishing if applicable.
This is a follow-on to TASK-020 (release workflow).

## Acceptance Criteria

- [ ] Decision made on which package registry to use (or explicit decision to skip)
- [ ] If publishing: workflow step added to TASK-020 release workflow
- [ ] If publishing: packages accessible and correctly versioned
- [ ] Decision and rationale documented in `## Notes`

## Notes

Dependency: TASK-020 (release workflow) should be completed first.
Publishing may not be necessary if GitHub Releases + direct download links are sufficient for this project.
