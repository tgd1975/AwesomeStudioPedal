---
id: TASK-015
title: Add compiler warning flags
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
---

## Description

Enable `-Wall -Wextra -Werror` (and additional pedantic flags) in the CMake build to treat
all compiler warnings as errors and enforce zero-warning policy.

## Acceptance Criteria

- [x] `-Wall -Wextra -Werror` added to host build (`CMakeLists.txt`)
- [x] All existing warnings resolved before enabling flags
- [x] CI fails on any new compiler warning

## Notes

Delivered in commit `dae06db`. All pre-existing warnings were resolved as part of this task.
See `CMakeLists.txt` for the active compile options.
