---
id: TASK-224
title: Test `housekeep.py --init` end-to-end in a fresh repository
status: closed
opened: 2026-04-23
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Support
epic: task-system
order: 17
prerequisites: [TASK-222]
---

## Description

Validate the full installation experience by running `housekeep.py --init` in a clean, temporary git repository — with no pre-existing task or idea files. Verify that the resulting folder structure is correct, all overview stubs are generated, and the system is immediately usable for creating tasks and ideas.

## Acceptance Criteria

- [ ] `housekeep.py --init` completes without errors in a fresh repo
- [ ] All expected folders are created (`open/`, `closed/`, `archive/`, `ideas/open/`, `ideas/archived/`)
- [ ] Stub `OVERVIEW.md`, `EPICS.md`, `KANBAN.md` are generated
- [ ] `/ts-task-new` can be run immediately after `--init` and creates a valid task file
- [ ] `/ts-idea-new` can be run immediately after `--init` and creates a valid idea file
- [ ] Document any issues found and fix them as follow-up

## Test Plan

**Host tests:** Extend `test/unit/test_housekeep.py` with a `--init` integration test in a temp directory.

## Prerequisites

- **TASK-222** — standalone layout must be finalized before testing it in isolation

## Notes

Support role: review the test results and record any rough edges found during testing as follow-up issues, not blockers.
