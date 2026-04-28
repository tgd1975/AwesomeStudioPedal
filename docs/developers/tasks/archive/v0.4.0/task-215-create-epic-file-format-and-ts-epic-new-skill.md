---
id: TASK-215
title: Define epic file format and create `ts-epic-new` skill
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 8
prerequisites: [TASK-213]
---

## Description

Define the epic file format (frontmatter + body sections) as specified in IDEA-021 and implement the `ts-epic-new` skill to scaffold new epic files. Epic files live alongside task files in `open/`, `active/`, and `closed/` and are moved automatically by `housekeep.py` based on derived status.

Epic frontmatter:

```yaml
id: EPIC-NNN
title: Short title
status: open|active|closed  # derived by housekeep.py — do not set manually
opened: YYYY-MM-DD
closed: YYYY-MM-DD          # set when all tasks closed
assigned: username           # optional
```

## Acceptance Criteria

- [ ] Epic file format is documented (inline in SKILL.md or in a separate doc)
- [ ] `.claude/skills/ts-epic-new/SKILL.md` exists and scaffolds a correctly formatted epic file
- [ ] `ts-epic-new` auto-increments EPIC-NNN across all folders
- [ ] `ts-epic-new` runs `housekeep.py --apply` after creation
- [ ] `housekeep.py` correctly reads epic files, derives their status, and moves them
- [ ] `ts-epic-new` is registered in `.vibe/config.toml`

## Test Plan

**Host tests:** Extend `test/unit/test_housekeep.py` to cover epic file parsing and status derivation from tasks.

## Prerequisites

- **TASK-213** — `housekeep.py` must be able to process epic files after the skill creates them

## Notes

Clarification needed: should `ts-epic-new` prompt for an assigned owner, or leave the field blank by default?
