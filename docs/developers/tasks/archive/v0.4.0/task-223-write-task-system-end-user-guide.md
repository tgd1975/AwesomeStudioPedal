---
id: TASK-223
title: Write `TASK_SYSTEM.md` end-user guide; add `VERSION` file and `housekeep.py --version` flag
status: closed
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Support
epic: task-system
order: 16
prerequisites: [TASK-222]
---

## Description

Write the end-user documentation for the task system as `docs/developers/TASK_SYSTEM.md`. This is a condensed, cleaned-up version of IDEA-021 — a practical guide for someone installing and using the system for the first time. Also add a `VERSION` file (e.g. `0.1.0`) and implement `housekeep.py --version` to print it.

## Acceptance Criteria

- [ ] `docs/developers/TASK_SYSTEM.md` exists and covers: prerequisites, installation steps, config options, skill reference, idea lifecycle, task lifecycle
- [ ] Guide is self-contained — a reader who has never seen IDEA-021 can follow it
- [ ] `VERSION` file exists at the repository root with initial version `0.1.0`
- [ ] `python scripts/housekeep.py --version` prints the version from the `VERSION` file
- [ ] Guide includes a note on how to update (git subtree pull or copy-paste)

## Test Plan

**Host tests:** No testable logic for the guide. Test `--version` flag manually.

## Prerequisites

- **TASK-222** — repository layout must be finalized before writing the guide for it

## Notes

Support role: review the guide for accuracy and completeness before publishing. Keep it under 4 pages — IDEA-021 is the design document; TASK_SYSTEM.md is the user manual.
