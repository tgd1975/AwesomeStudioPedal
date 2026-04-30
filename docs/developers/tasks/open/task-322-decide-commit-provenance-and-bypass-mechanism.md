---
id: TASK-322
title: Decide commit-provenance signal and bypass mechanism for /commit
status: open
opened: 2026-04-30
effort: Small (<2h)
complexity: Senior
human-in-loop: Main
epic: commit-atomicity
order: 1
---

## Description

Resolve two open questions from IDEA-051 that block the rest of the epic:

1. **Provenance signal** — what does `/commit` set so the pre-commit hook
   can prove "this commit came from /commit, not raw git"? Candidates:
   - Env var with short-lived token (simple, but env vars inherit across
     processes — risk of leaking provenance to unrelated commits).
   - Commit trailer (visible in history, but pollutes commit messages).
   - File marker in `.git/` (cleanly scoped, but needs cleanup on crash).
2. **Bypass mechanism** — how does a developer legitimately bypass the
   `/commit`-only rule for manual repo surgery, recovery, rebases?
   `ASP_COMMIT_BYPASS=1` env var is the strawman from IDEA-051 — confirm
   or replace, and define who is allowed to use it.

Output is a short decision document captured in the task body (an ADR-style
note) that TASK-323 / TASK-325 can reference. No code changes here.

## Acceptance Criteria

- [ ] Provenance signal chosen, with one-paragraph rationale covering
      inheritance/leak risk, crash-cleanup behaviour, and visibility.
- [ ] Bypass mechanism named, with the exact env var or flag, the
      intended use cases, and how misuse will be detected (e.g. logged
      to a file the user reviews periodically).
- [ ] Decisions written into this task body under a `## Decision` section
      so subsequent tasks can cite it.

## Test Plan

No automated tests required — change is non-functional (decision document only).

## Notes

- Sequencing: this is the first task in EPIC-022. TASK-323 (pathspec
  conversion) does not strictly depend on it, but TASK-324 (CLAUDE.md +
  COMMIT_POLICY.md) and TASK-325 (hook enforcement) both do.
- Risk: choosing an env var that leaks across child processes (e.g. a
  hook that itself spawns `git commit`) creates false positives. The
  decision should explicitly address this.
