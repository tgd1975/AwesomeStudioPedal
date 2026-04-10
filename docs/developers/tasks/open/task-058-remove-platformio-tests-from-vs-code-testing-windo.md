---
id: TASK-058
title: Remove PlatformIO Tests from VS Code testing window
status: open
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

PlatformIO's on-device tests appear in the VS Code Testing window alongside the host unit tests,
causing confusion. Configure the workspace so only the CMake/CTest host tests are shown there.

## Acceptance Criteria

- [ ] VS Code Testing window shows only host unit tests (CMake/CTest)
- [ ] PlatformIO on-device tests are not listed in the Testing window
- [ ] On-device tests can still be run via PlatformIO UI or CLI as before
- [ ] Solution documented in `.vscode/` settings or `## Notes`

## Notes

Likely fix: adjust `.vscode/settings.json` to disable the PlatformIO test discovery extension
or configure the test include/exclude patterns.
Check `platformio.ini` test filter settings as an alternative approach.
