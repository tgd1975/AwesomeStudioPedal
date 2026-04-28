---
id: TASK-287
title: Add /status skill bundling branch + last 3 commits + git status --short
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: XS (<30m)
effort_actual: XS (<30m)
complexity: Junior
human-in-loop: No
epic: agent-collaboration-skills
order: 5
---

## Description

Across 148 transcripts the agent ran the same small reconnaissance bundle
hundreds of times:

- `git status` / `git status --short` — 163 invocations
- `git log --oneline -3` (and `-5`, `-8`, `-10` variants) — 100+
- `git rev-parse --abbrev-ref HEAD` / `git branch --show-current` — 58
- Composite forms like `git log --oneline -3 && git status --short` — 33

Each is a separate Bash call requiring a permission prompt. A trivial
`/status` skill that runs all three in one Bash invocation and prints a
compact four-line report (branch, last 3 commits, staged short, working
short) would shave dozens of clicks across sessions.

## Acceptance Criteria

- [x] `.claude/skills/status/SKILL.md` exists and is registered in
      `.vibe/config.toml`.
- [x] One invocation runs all four commands and prints a clearly-sectioned
      report.
- [x] CLAUDE.md gains a one-liner: "use `/status` for routine git
      reconnaissance instead of separate calls."

## Test Plan

No automated tests required — change is non-functional skill scaffolding.

## Notes

- Lowest-leverage item in the epic but also the cheapest. Worth doing
  alongside the others.
- Source signal: 300+ git-recon Bash calls in transcript corpus.
