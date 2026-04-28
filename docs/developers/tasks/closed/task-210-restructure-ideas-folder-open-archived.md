---
id: TASK-210
title: Restructure ideas folder — create `open/` and `archived/` subdirectories and migrate existing files
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
epic: task-system
order: 3
prerequisites: [TASK-209]
---

## Description

Create the `docs/developers/ideas/open/` and `docs/developers/ideas/archived/` subdirectories and move all existing idea files into `open/`. This establishes the directory-as-status convention where file location replaces a status frontmatter field.

## Acceptance Criteria

- [ ] `docs/developers/ideas/open/` directory exists and contains all current open idea files
- [ ] `docs/developers/ideas/archived/` directory exists (may be empty initially)
- [ ] No idea files remain flat in `docs/developers/ideas/` (only subdirectories and OVERVIEW.md)
- [ ] Any existing references to idea file paths in scripts or docs are updated

## Test Plan

**Host tests:** No logic changes — verify by directory listing after migration.

## Notes

Check whether `update_future_ideas.py` (the predecessor script) references flat paths — update those references in TASK-211.
