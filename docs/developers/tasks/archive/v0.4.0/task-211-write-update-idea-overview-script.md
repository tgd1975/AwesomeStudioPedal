---
id: TASK-211
title: Write `scripts/update_idea_overview.py` — generates `ideas/OVERVIEW.md`, replaces `update_future_ideas.py`
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Clarification
epic: task-system
order: 4
prerequisites: [TASK-210]
---

## Description

Write a new Python script that replaces the existing `update_future_ideas.py`. The new script scans `docs/developers/ideas/open/` and generates a clean `OVERVIEW.md` listing all open ideas. Optionally (controlled by the `inject` config key, disabled by default) it can also update a named block in another file such as `README.md`.

## Acceptance Criteria

- [ ] `scripts/update_idea_overview.py` exists and is executable
- [ ] Script reads all `.md` files from `ideas/open/`, parses their frontmatter (`id`, `title`, `description`)
- [ ] Generates `docs/developers/ideas/OVERVIEW.md` with a table of open ideas
- [ ] Old `update_future_ideas.py` is removed or clearly deprecated
- [ ] Script accepts `--dry-run` flag (prints output without writing)
- [ ] Script is idempotent — running twice produces the same result

## Test Plan

**Host tests:** Add `test/unit/test_update_idea_overview.py` (pytest) — cover: empty folder, single idea, multiple ideas, missing description field.

## Prerequisites

- **TASK-210** — ideas must be in `open/` folder before the script can scan them

## Notes

Clarification needed: confirm whether the README.md injection should be wired up now or deferred until the config system (Phase 4) is in place.
