---
id: TASK-328
title: Prevent parallel execution of housekeep.py (and audit sibling scripts)
status: open
opened: 2026-04-30
effort: Small (<2h)
complexity: Medium
human-in-loop: Clarification
---

## Description

`scripts/housekeep.py` can be triggered from several independent paths in this repo:
the pre-commit hook, the `/housekeep` skill, the `/ts-task-active` / `/ts-task-done` /
`/ts-task-pause` / `/ts-task-reopen` skills (which all invoke housekeep internally),
and direct manual invocation. Parallel Claude Code sessions are common in this project
(see CLAUDE.md "Parallel sessions"), which means two housekeep processes can race —
both reading the same task tree, both rewriting `OVERVIEW.md` / `EPICS.md` / `KANBAN.md`
/ ideas `OVERVIEW.md`, and the last writer wins with possibly stale or interleaved
content.

The fix is a single-instance guard: when a second housekeep run starts while one is
already in flight, it must not silently produce a half-correct index.

The task also includes a quick audit of sibling scripts that have similar trigger
fan-out and thus the same race risk: `sync_task_system.py` (pre-commit + manual +
skill paths), `update_task_overview.py`, `update_idea_overview.py`,
`organize_closed_tasks.py`. If any of them share the same risk profile, apply the
same lock pattern; otherwise document why they are safe (e.g. read-only).

The user explicitly raised three candidate behaviours when a second instance starts:

1. **Fail silently** — return success, do nothing. Lowest friction, but masks bugs.
2. **Wait** — block until the in-flight run finishes, then run again (or skip if the
   work is already done by the first run).
3. **Fail loudly** — exit non-zero with a clear message that another instance holds
   the lock.

Picking the right one is part of this task. Recommended starting position: **wait
with a short timeout, then fail loudly**, because (a) the pre-commit hook must not
swallow regen failures, (b) silent skips desynchronise the index from the moved
file, and (c) a true second-run *triggered after* the first finishes is the common
case and should "just work" (which a wait-then-run pattern delivers naturally).

## Acceptance Criteria

- [ ] `housekeep.py` acquires an exclusive lock (e.g. `fcntl.flock` on a pidfile in
      a writable, repo-local path) before doing any index regeneration; releases it
      on exit (success or failure).
- [ ] Concurrent invocation behaviour is decided, documented in the script's module
      docstring, and matches the implemented behaviour (the recommended default is
      "wait up to N seconds, then fail with a clear message naming the lock holder's
      PID").
- [ ] A host test exercises the concurrency path — e.g. spawn two `housekeep.py`
      subprocesses, assert exactly one regenerates the index and the other observes
      the lock as expected (waits, or fails with the documented exit code/message).
- [ ] Audit decision recorded for `sync_task_system.py`,
      `update_task_overview.py`, `update_idea_overview.py`,
      `organize_closed_tasks.py`: either the same lock pattern is applied, or the
      script is documented as safe (read-only, idempotent, or always-serialised by
      its caller).
- [ ] `awesome-task-system/scripts/housekeep.py` is updated identically and
      `python scripts/sync_task_system.py --check` passes.

## Test Plan

**Host tests** (Python — there is no existing pytest harness for `scripts/`,
so add one alongside the existing `awesome-task-system/scripts/tests/test_housekeep.py`):

- Extend `awesome-task-system/scripts/tests/test_housekeep.py` (or add a new
  `test_housekeep_concurrency.py` next to it) with a test that:
  1. Acquires the lock manually in the test process.
  2. Spawns `housekeep.py --apply` as a subprocess.
  3. Asserts the documented behaviour (waits and then runs, or fails non-zero
     with a recognisable message).
- After implementation, run `python scripts/sync_task_system.py --apply` and confirm
  the package and live copies match.

No on-device tests — pure host-side Python.

## Notes

- **Lock file location.** Pick something inside the repo that is gitignored
  (e.g. `.housekeep.lock` at repo root, added to `.gitignore`) rather than `/tmp`,
  because `/tmp` is shared across repos on Linux and would falsely serialise
  housekeep runs from unrelated checkouts. On Windows (Git Bash / MSYS2) `fcntl`
  is unavailable — use `msvcrt.locking` or a portable shim like the `filelock`
  package; the project already targets both OSes per CLAUDE.md.
- **Pre-commit hook interaction.** If housekeep is invoked twice within the same
  commit (e.g. once by a skill before the commit, once by the hook), the second
  call should be a near-no-op because the index is already up to date. A "wait
  then run" implementation handles this naturally; a "fail silently" one would
  too, but at the cost of hiding real bugs elsewhere.
- **Stale lock recovery.** If a previous run died without releasing (Ctrl-C,
  killed process), a stale lockfile on disk should not wedge future runs.
  `fcntl.flock` releases automatically when the process exits — so as long as
  the lock is acquired via `flock` (not just by-existence-of-file), this is free.
  Document this explicitly in the docstring so future readers do not "fix" it.
- **Out of scope.** This task does not introduce a generic locking helper for all
  scripts under `scripts/` — only the four named in the audit. A library-style
  abstraction is premature until at least two scripts share the same lock shape.
- **Source-of-truth reminder.** Per CLAUDE.md, edit `awesome-task-system/scripts/housekeep.py`
  first, then run `sync_task_system.py --apply` to mirror to `scripts/`.
