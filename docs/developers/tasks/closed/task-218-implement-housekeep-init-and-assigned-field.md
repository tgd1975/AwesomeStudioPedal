---
id: TASK-218
title: Implement `housekeep.py --init` for first-time setup; wire `assigned` field into all generated views
status: closed
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 11
prerequisites: [TASK-217]
---

## Description

Two related configuration-phase deliverables:

1. **`housekeep.py --init`** — creates all required folder structure and stub overview files for a fresh project. Reads from config to determine which subsystems are enabled, then creates only the folders and files that are needed.

2. **`assigned` field** — wire the `assigned:` frontmatter field into all generated views: OVERVIEW.md (extra column, shown only if at least one task has it set), KANBAN.md (badge on card), EPICS.md (epic header row).

## Acceptance Criteria

- [ ] `python scripts/housekeep.py --init` creates `open/`, `active/`, `closed/`, `archive/` under `tasks.base_folder` (respecting config flags)
- [ ] `--init` creates `open/` and `archived/` under `ideas.base_folder` (if ideas enabled)
- [ ] `--init` generates stub `OVERVIEW.md`, `EPICS.md` (if epics enabled), `KANBAN.md` (if kanban enabled)
- [ ] `--init` is idempotent — safe to run in a repo that already has the structure
- [ ] OVERVIEW.md shows `Assigned` column when at least one task has the field set
- [ ] KANBAN.md shows `@username` badge on cards with `assigned` set
- [ ] EPICS.md shows assigned owner in epic header row

## Test Plan

**Host tests:** Extend `test/unit/test_housekeep.py` — cover: `--init` in empty dir, `--init` with existing structure (idempotent), `assigned` column appears only when field present.

## Prerequisites

- **TASK-217** — config file and config-aware scripts must exist before `--init` can read from them

## Notes

Clarification needed: should `--init` overwrite existing overview files or skip them? Recommendation: skip and print a warning.
