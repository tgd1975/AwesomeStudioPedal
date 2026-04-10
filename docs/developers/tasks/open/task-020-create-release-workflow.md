---
id: TASK-020
title: Create release workflow
status: open
opened: 2026-04-10
effort: Extra Large (24-40h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Create `.github/workflows/release.yml` to automate the release process:
build, test, generate changelog, create the GitHub release, and upload firmware assets.

## Acceptance Criteria

- [ ] Workflow file exists at `.github/workflows/release.yml`
- [ ] Triggers on manual dispatch or version tag push (`v*`)
- [ ] Builds firmware for both ESP32 and nRF52840
- [ ] Runs all tests before publishing
- [ ] Generates changelog from commit messages
- [ ] Creates a GitHub release with tag and release notes
- [ ] Uploads firmware binaries as release assets using the naming convention below
- [ ] CI passes on the PR that adds this workflow

## Notes

Firmware asset naming convention:

```
awesome-pedal-[platform]-[version].bin
# Examples:
awesome-pedal-esp32-v1.2.0.bin
awesome-pedal-nrf52840-v1.2.0.bin
```

Consider two trigger modes:

- Manual (`workflow_dispatch`) for fully controlled releases
- Tag push (`on: push: tags: ['v*']`) for automated releases

Related: TASK-021 (package publishing), TASK-022 (release checklist), TASK-023 (CONTRIBUTING.md release docs), TASK-029 (release cleanup script).
