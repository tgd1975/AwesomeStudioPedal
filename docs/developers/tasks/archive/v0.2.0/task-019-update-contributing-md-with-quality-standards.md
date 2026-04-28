---
id: TASK-019
title: Update CONTRIBUTING.md with quality standards
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add a "Code Quality Standards" section to `CONTRIBUTING.md` documenting the zero-warnings
policy, clang-tidy requirement, clang-format enforcement, code smell rules, and test coverage
expectation.

## Acceptance Criteria

- [x] "Code Quality Standards" section added to `CONTRIBUTING.md`
- [x] Zero-warnings policy documented (`-Werror`)
- [x] Static analysis requirement documented (run `clang-tidy` locally before submitting)
- [x] Code smell rules documented (no magic numbers, deep nesting, long functions)
- [x] Test coverage requirement documented (new behaviour needs a host unit test)

## Notes

Delivered in commit `d6f9e90`. See [CONTRIBUTING.md](../../../CONTRIBUTING.md).
