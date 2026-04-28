---
id: TASK-288
title: Add CLAUDE.md guidance to prefer Explore subagent for multi-step searches
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: XS (<30m)
effort_actual: XS (<30m)
complexity: Junior
human-in-loop: No
epic: agent-collaboration-skills
order: 6
---

## Description

Across 148 transcripts the agent invoked `grep` 992 times and `find` 219
times, while the `Agent` tool (any subagent) was called only **14 times**
total. A meaningful fraction of those greps/finds were "search the
codebase for X across multiple naming conventions or directories" — the
exact use case the `Explore` subagent exists for.

Each Bash search is a permission prompt and a context-window hit; one
Explore call covers many lookups and returns a synthesised result. The
session cost difference is large.

This task adds a short CLAUDE.md section that names the heuristic
explicitly: **for any search that will likely take more than 3 grep/find
queries, use `Agent(subagent_type="Explore")` instead of hand-rolling
the searches.** The system prompt already hints at this — the goal is to
make the heuristic visible at the project level so it actually gets
applied.

## Acceptance Criteria

- [x] CLAUDE.md gains a short "Searching the codebase" section with the
      3-query rule of thumb and a one-line example of when to delegate
      to Explore.
- [x] The section is short (≤8 lines) so it does not bloat CLAUDE.md.

## Test Plan

No automated tests required — pure documentation change.

## Notes

- Source signal: 1211 raw `grep`/`find` calls vs 14 `Agent` calls in the
  transcript corpus.
- This is a behaviour nudge, not a tooling change. If it does not work
  (the agent keeps reaching for grep), promote to a stronger trigger.
