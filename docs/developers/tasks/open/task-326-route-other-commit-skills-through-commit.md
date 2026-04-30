---
id: TASK-326
title: Route /release, /release-branch, and other commit-issuing skills through /commit
status: open
opened: 2026-04-30
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
epic: commit-atomicity
order: 5
prerequisites: [TASK-325]
---

## Description

Once the pre-commit hook from TASK-325 lands, every skill that issues
`git commit` directly will start failing. Audit and fix:

- `/release`
- `/release-branch`
- `/ts-task-active`, `/ts-task-done`, `/ts-task-pause`, `/ts-task-reopen`
  (these run housekeep but historically may not commit themselves —
  verify)
- `/housekeep` (verify — it stages but should not commit)
- Any other skill under `.claude/skills/` that calls `git commit`

For each skill, the choice is:

1. **Route through `/commit`** — call `/commit` rather than raw `git
   commit`. Preferred when the skill knows exactly which files it owns.
2. **Use the documented bypass from TASK-322** — when the skill performs
   genuinely-multi-file atomic operations (release tagging, squash-merge)
   that the `/commit` flow doesn't model. Each bypass site must be
   justified in the skill prose so future readers understand why.

Risk to manage: "every skill has bypass, so the rule is meaningless."
Keep the bypass list short and reviewed.

## Acceptance Criteria

- [ ] Audit produced — list of every skill that issues `git commit`,
      with the chosen route (through `/commit` or bypass) and a one-line
      justification per bypass.
- [ ] Each affected skill updated to either route through `/commit` or
      set the bypass mechanism, matching the audit.
- [ ] Smoke test of each affected skill passes against the live hook
      (i.e. exercise `/release-branch` end-to-end on a throwaway branch).

## Test Plan

No automated tests required — change is non-functional (skill prose plus
manual smoke tests). Each affected skill is verified by running it once
end-to-end and confirming the commit succeeds without disabling the hook.

## Prerequisites

- **TASK-325** — the hook must be live before this task can audit
  what breaks. Trying to do this first means guessing which skills
  bypass-need versus route-need.

## Notes

- The `awesome-task-system` package mirror (`scripts/sync_task_system.py`)
  copies skill files between `awesome-task-system/` and `.claude/skills/`
  — any skill edits here must be made on the package side first per
  CLAUDE.md.
