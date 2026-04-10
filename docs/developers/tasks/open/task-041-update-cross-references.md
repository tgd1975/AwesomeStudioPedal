---
id: TASK-041
title: Update cross-references
status: open
effort: Medium (2-8h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Audit all internal documentation cross-references and ensure every link between markdown files
is correct, uses consistent anchor format, and points to files that still exist.

## Acceptance Criteria

- [ ] All relative markdown links between docs verified and working
- [ ] Anchors (`#section-name`) match actual headings (case-insensitive, spaces → hyphens)
- [ ] No broken or stale references to deleted/renamed files
- [ ] `README.md` links to the correct documentation files

## Notes

Focus areas: `README.md`, `CONTRIBUTING.md`, `docs/developers/`, `docs/building.md` (once created).
Use a link checker or manually audit. Consider running `markdown-link-check` as part of CI (TASK-042 covers external links).
