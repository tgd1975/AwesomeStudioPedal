---
id: TASK-146
title: Update Release Workflow and README
status: closed
closed: 2026-04-19
opened: 2024-10-08
effort: Small (<2h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
epic: Release
order: 1
---

## Description

Update the release workflow (skills, descriptions, etc.) to ensure the firmware download section in the main `README.md` is visible and correctly formatted. Currently, the firmware download section is hidden or commented out and needs to be uncommented and updated to reflect the actual release artifacts.

## Acceptance Criteria

- [ ] The firmware download section in `README.md` is uncommented and visible.
- [ ] The section includes correct links to the latest release artifacts for both ESP32 and nRF52840 platforms.
- [ ] The release workflow documentation is updated to reflect the changes.
- [ ] The `release` skill is updated to ensure the README is correctly updated during the release process.

## Notes

The firmware download section is currently hidden in the README. It should be uncommented and updated to reflect the actual release artifacts. The release workflow should also be updated to ensure this section is correctly maintained during future releases.
