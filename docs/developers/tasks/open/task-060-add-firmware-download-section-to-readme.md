---
id: TASK-060
title: Add firmware download section to README
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: No
---

## Description

Once the first public release is tagged, restore a proper "Firmware" section in `README.md`
with real download links pointing to GitHub Release assets.

## Acceptance Criteria

- [ ] A GitHub Release exists with firmware binaries attached
- [ ] `README.md` "Firmware" section lists current stable version with working download links
- [ ] Links follow naming convention: `awesome-pedal-[platform]-[version].bin`
- [ ] Previous versions section lists up to 2 prior releases (or omitted if first release)

## Notes

The placeholder firmware section was removed before the first release (see commit on 2026-04-10)
because the v1.2.0 links in README were stale (only v0.1.0 was tagged).

Do this task immediately after tagging the first public release via TASK-020 (release workflow).
See TASK-025 for the full firmware versions section design, TASK-029 for the cleanup script.
