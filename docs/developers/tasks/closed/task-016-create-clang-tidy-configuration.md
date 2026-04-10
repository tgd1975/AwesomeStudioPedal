---
id: TASK-016
title: Create .clang-tidy configuration
status: closed
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Create a `.clang-tidy` configuration file enabling a comprehensive set of checks
(`bugprone-*`, `clang-analyzer-*`, `cppcoreguidelines-*`, `modernize-*`, `performance-*`,
`readability-*`) with a curated set of suppressions for patterns unavoidable in this codebase.

## Acceptance Criteria

- [x] `.clang-tidy` created at repository root
- [x] All enabled checks pass on existing source files
- [x] `WarningsAsErrors` set to catch issues in CI

## Notes

Delivered in commit `ffd70b4`. See [.clang-tidy](../../../.clang-tidy).
Notable suppressions include `cppcoreguidelines-avoid-c-arrays`, `pro-bounds-*`,
and `cppcoreguidelines-owning-memory` due to Arduino/PlatformIO idioms.
