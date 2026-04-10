---
id: TASK-056
title: Revisit firmware download links
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Review all firmware download links in `README.md` and documentation to ensure they point to
actual existing GitHub Release assets and are not stale or broken.

## Acceptance Criteria

- [ ] All firmware download links verified as reachable
- [ ] Links updated to point to the latest stable release
- [ ] Naming convention consistent: `awesome-pedal-[platform]-[version].bin`
- [ ] Old or dangling links removed

## Notes

Related to TASK-025 (add firmware versions section) — if that task is done first, this task
becomes a periodic maintenance check rather than an initial setup.
Use `gh release list` and `gh release view` to inspect available assets.
