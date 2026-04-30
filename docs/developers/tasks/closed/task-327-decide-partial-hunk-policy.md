---
id: TASK-327
title: Decide partial-hunk policy — unsupported, or /commit --partial escape hatch
status: open
opened: 2026-04-30
effort: Small (<2h)
complexity: Medium
human-in-loop: Main
epic: commit-atomicity
order: 6
prerequisites: [TASK-325]
---

## Description

Pathspec form is whole-file only — `git add -p` partial hunks are not
expressible as `git commit <files> -m`. IDEA-051 leaves this as an open
question:

- **Option A — unsupported.** Document partial-hunk committing as not
  supported in parallel sessions. Developers who want partial hunks
  must serialise (one Claude session at a time) or split the file
  manually first.
- **Option B — `/commit --partial` escape hatch.** Add a `--partial`
  mode to `/commit` that acquires a `flock` on a project lockfile,
  runs `git add -p`, commits, releases the lock. More work, but
  preserves a workflow some developers rely on.

Decide which, document it, and implement only if Option B.

## Acceptance Criteria

- [ ] Decision documented in `docs/developers/COMMIT_POLICY.md` (created
      in TASK-324) with the rationale.
- [ ] If Option A: `/commit` skill explicitly states partial-hunk
      committing is not supported and points users at the workaround
      (split the file or serialise sessions).
- [ ] If Option B: `/commit --partial` implemented, with `flock` on a
      project-local lockfile, and tested end-to-end.

## Test Plan

**If Option A chosen:** No automated tests required — change is
non-functional (docs).

**If Option B chosen:** Manual end-to-end verification that two
concurrent `/commit --partial` invocations correctly serialise via
`flock` and neither corrupts the other's commit. Document the
verification in the task closing notes.

## Prerequisites

- **TASK-325** — the hook enforcement context determines whether a
  partial-hunk path needs to provide its own provenance signal or can
  inherit `/commit`'s.

## Notes

- Default expectation: Option A. Partial-hunk committing is rare in
  Claude sessions, and the `flock` machinery in Option B is real
  surface area for bugs (stale lockfiles, crash recovery). Choose B
  only if there is a demonstrated workflow that needs it.
