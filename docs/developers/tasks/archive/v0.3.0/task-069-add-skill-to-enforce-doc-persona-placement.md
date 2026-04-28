---
id: TASK-069
title: Add skill to enforce documentation persona placement
status: closed
opened: 2026-04-11
closed: 2026-04-11
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
---

## Description

The project uses a strict three-persona documentation layout:

| Directory | Audience |
|-----------|----------|
| `docs/musicians/` | End-users operating the pedal |
| `docs/builders/` | Hardware builders assembling the device |
| `docs/developers/` | Contributors and developers |

Content drifts into the wrong directory over time, making it harder for each
audience to find relevant information. This task adds a Claude Code skill —
`/doc-check` — that inspects recently changed or newly added `.md` files,
identifies any that appear to be in the wrong persona directory based on their
path and content, and advises the author on the correct location.

The skill should be a lightweight guide tool, not a hard linter. It reads the
diff (or a provided list of files), summarises which persona each file is aimed
at, and flags mismatches with a short explanation and a suggested destination.

## Acceptance Criteria

- [ ] `/doc-check` skill exists in `.claude/skills/doc-check/SKILL.md`
- [ ] The skill checks `.md` files changed relative to `main` (via
  `git diff --name-only main...HEAD -- '*.md'`) and classifies each into one
  of the three personas based on path and content heuristics
- [ ] Files that appear to be in the wrong persona directory are flagged with
  the suggested correct path; files that look correct are confirmed as-is
- [ ] Skill is added to the skills table in `CONTRIBUTING.md`

## Notes

- A single `/doc-check` skill is preferable to three separate per-persona
  skills — the value is in cross-persona comparison, not per-directory checks
- Content heuristics to consider: mentions of "build", "solder", "wiring" →
  builder; "press button", "profile", "Bluetooth" as end-user concepts →
  musician; "CMake", "PR", "architecture", "clang" → developer
- Skill does not move or rename files — it only advises; the author acts
- If no `.md` files changed, report "No documentation changes to check."
- Consider whether `docs/developers/tasks/` files should be excluded (they
  are internal scaffolding, not user-facing documentation)
