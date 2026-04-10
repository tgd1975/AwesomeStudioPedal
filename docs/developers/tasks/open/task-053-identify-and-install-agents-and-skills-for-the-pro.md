---
id: TASK-053
title: Identify and install agents and skills for the project
status: open
opened: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Audit what Claude Code agents and skills would be useful for this project, then implement and
install them under `.claude/skills/`. The goal is to make routine developer workflows faster.

## Acceptance Criteria

- [ ] Existing skills (`/tasks`, `/task-new`, `/task-done`) reviewed and refined if needed
- [ ] At least one new useful skill identified and implemented
- [ ] All skills documented in `.claude/skills/<name>/SKILL.md`
- [ ] Skills verified to pass markdownlint
- [ ] Skills listed or referenced in `CONTRIBUTING.md` or developer docs

## Notes

Existing skills (as of TASK-053 creation):

- `/tasks` — list open tasks
- `/task-new` — scaffold a new task file
- `/task-done` — close a task and update OVERVIEW.md

Candidates for new skills: `/release` (tag + trigger workflow), `/lint` (run all linters),
`/test` (run host tests), or domain-specific helpers.
