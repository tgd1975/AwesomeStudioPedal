---
id: TASK-217
title: Create `docs/developers/task-system.yaml` and make all scripts and skills config-aware
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Support
epic: task-system
order: 10
prerequisites: [TASK-216]
---

## Description

Create the single config file at `docs/developers/task-system.yaml` and update all scripts (`housekeep.py`, `update_idea_overview.py`) and skills to read from it. The config controls which subsystems are enabled, where folders are located, and whether optional features like `active/` state and releases are on.

Config structure (from IDEA-021):

```yaml
ideas:
  enabled: true
  base_folder: docs/developers/ideas
tasks:
  enabled: true
  base_folder: docs/developers/tasks
  active:
    enabled: true
  releases:
    enabled: true
  epics:
    enabled: true
scripts:
  base_folder: scripts
visualizations:
  epics:
    enabled: true
    style: dependency-graph
  kanban:
    enabled: true
```

## Acceptance Criteria

- [ ] `docs/developers/task-system.yaml` exists with documented default values
- [ ] `housekeep.py` reads all relevant config keys; respects `tasks.active.enabled`, `tasks.epics.enabled`
- [ ] `update_idea_overview.py` reads `ideas.base_folder` from config
- [ ] All skills read config path from `TASK_SYSTEM_CONFIG` env var (default: `docs/developers/task-system.yaml`)
- [ ] Scripts warn (not fail) if config file is missing — fall back to hardcoded defaults

## Test Plan

**Host tests:** Extend `test/unit/test_housekeep.py` — cover: config with `tasks.active.enabled: false`, config with `tasks.epics.enabled: false`, missing config file falls back to defaults.

## Prerequisites

- **TASK-216** — all Phase 3 work must be complete before making it config-driven

## Notes

Support role: confirm which config keys each script and skill needs, and agree on the fallback defaults. Use PyYAML for parsing; it's already a project dependency.
