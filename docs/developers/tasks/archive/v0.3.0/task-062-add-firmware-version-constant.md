---
id: TASK-062
title: Add firmware version constant
status: closed
closed: 2026-04-16
opened: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: No
---

## Description

Define a `FIRMWARE_VERSION` constant in the firmware so the running version is
accessible at runtime (e.g. via serial output on boot).

## Acceptance Criteria

- [ ] `FIRMWARE_VERSION` string constant defined (e.g. in `src/version.h`)
- [ ] Version printed to serial on boot
- [ ] Version string follows `vX.Y.Z` convention and matches the git tag
- [ ] Bumping the version for a release means editing one file only

## Notes

Consider whether to use a manual `version.h` or generate it from the git tag
via a PlatformIO build script (`extra_scripts`). The manual approach is simpler;
the generated approach avoids version drift.

Do this task before or during the next release cycle so the version is visible
in serial output during hardware testing.
