---
id: TASK-212
title: Create `ts-idea-new`, `ts-idea-list`, `ts-idea-archive` skills and register in `.vibe/config.toml`
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 5
prerequisites: [TASK-211]
---

## Description

Implement the three idea management skills following the `ts-<domain>-<verb>` naming convention. Each skill is a SKILL.md file under `.claude/skills/`. All three must be registered in `.vibe/config.toml` under `enabled_skills`.

- **`ts-idea-new "Title"`** — creates a new idea file in `ideas/open/`, auto-increments IDEA-NNN, runs `update_idea_overview.py`. Does not commit.
- **`ts-idea-list`** — reads `ideas/open/` and displays all open ideas (id, title, description).
- **`ts-idea-archive IDEA-NNN`** — moves idea from `open/` to `archived/`, records reason in file, runs `update_idea_overview.py`. Commits.

## Acceptance Criteria

- [ ] `.claude/skills/ts-idea-new/SKILL.md` exists and works end-to-end
- [ ] `.claude/skills/ts-idea-list/SKILL.md` exists and lists open ideas correctly
- [ ] `.claude/skills/ts-idea-archive/SKILL.md` exists and moves file + commits
- [ ] All three skills are listed in `.vibe/config.toml` `enabled_skills`
- [ ] `/ts-idea-archive` produces a commit with a meaningful message
- [ ] Invoking `/ts-idea-new` with no title prompts for one

## Test Plan

**Host tests:** Skill SKILL.md files contain no testable logic — manual smoke test each skill in a scratch directory.

## Prerequisites

- **TASK-211** — `update_idea_overview.py` must exist before skills can invoke it

## Notes

Clarification needed: should `ts-idea-archive` ask for a reason/note before archiving, or is moving the file sufficient and the user adds context manually?
