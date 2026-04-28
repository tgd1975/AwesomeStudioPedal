---
id: TASK-222
title: Restructure scripts and skills into standalone `awesome-task-system` repository layout
status: closed
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Support
epic: task-system
order: 15
prerequisites: [TASK-221]
---

## Description

Extract all task system scripts and skills into a directory layout that can be published as a standalone GitHub repository (`awesome-task-system`). The layout should allow users to copy the relevant folders into their project and start using the system immediately. A `--init` flag on `housekeep.py` handles first-time setup.

Artifacts to include:

- `scripts/housekeep.py`
- `scripts/update_idea_overview.py`
- `.claude/skills/ts-task-*/`
- `.claude/skills/ts-epic-*/`
- `.claude/skills/ts-idea-*/`
- `.claude/skills/ts-release/` (if implemented)
- `docs/developers/task-system.yaml` (config template)
- `docs/developers/TASK_SYSTEM.md` (end-user guide)

## Acceptance Criteria

- [ ] Repository layout is defined and documented
- [ ] All scripts and skills are self-contained (no implicit dependencies on this project's structure)
- [ ] `task-system.yaml` config template is included with all options documented
- [ ] `housekeep.py --init` works correctly in the standalone layout
- [ ] No references to AwesomeStudioPedal-specific paths or conventions remain in the distributed artifacts

## Test Plan

**Host tests:** Run `housekeep.py --init` in a temporary directory and verify it creates the expected structure.

## Prerequisites

- **TASK-221** — all Phase 5 work (visualization generation and list skills) must be complete before packaging

## Notes

Support role: decide which files to include vs. keep project-internal. The `ts-release` skill (used for versioned archive folders) is a candidate for inclusion but may need cleanup before publishing.
