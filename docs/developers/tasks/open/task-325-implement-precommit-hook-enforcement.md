---
id: TASK-325
title: Implement pre-commit hook enforcement of /commit-only commits
status: open
opened: 2026-04-30
effort: Medium (2-8h)
complexity: Senior
human-in-loop: Main
epic: commit-atomicity
order: 4
prerequisites: [TASK-322, TASK-323]
---

## Description

Add a pre-commit hook check that **rejects any commit that did not flow
through `/commit`**. The hook reads the provenance signal defined in
TASK-322 and, if absent, refuses the commit with a message naming the
rule and pointing at `/commit`.

The same hook also rejects `git commit -a` regardless of source —
pathspec scoping is bypassed by `-a`, so the parallel-session safety
property collapses if `-a` is allowed.

Bypass via the mechanism from TASK-322 (e.g. `ASP_COMMIT_BYPASS=1`) is
honoured but logged so misuse can be reviewed.

This is Pillar 3 of IDEA-051 — the load-bearing one. Without it, Pillars
1 and 2 are polite suggestions that the next session forgets.

## Acceptance Criteria

- [ ] Pre-commit hook rejects raw `git commit -m "..."` outside `/commit`
      with a message naming the rule and pointing at `/commit`.
- [ ] Pre-commit hook rejects `git commit -a` regardless of provenance.
- [ ] Bypass via the TASK-322 mechanism succeeds and logs the bypass
      (file under `.git/` or stderr line — defined in TASK-322).
- [ ] All existing pre-commit hook checks (housekeep regen, sync_task_system,
      tests) continue to run after the new check.

## Test Plan

**Host tests** (`make test-host`):

- Add `test/unit/test_commit_provenance_hook.cpp` only if the hook is
  implemented in C++. If the hook is a shell or Python script (likely),
  add a script-level test under `scripts/tests/` or equivalent.
- Cover:
  - raw `git commit` rejected
  - `git commit -a` rejected even with provenance signal present
  - `/commit`-style invocation with provenance signal accepted
  - bypass mechanism accepted and logged

If a script-level test harness does not yet exist, the test plan may be
satisfied by a documented manual verification checklist in the task
closing notes — but a real automated test is strongly preferred.

## Prerequisites

- **TASK-322** — defines the provenance signal the hook checks for and
  the bypass mechanism the hook honours.
- **TASK-323** — `/commit` must already be setting the provenance signal
  before the hook starts requiring it, otherwise every commit fails.

## Notes

- Sequencing matters: TASK-323 lands first (so `/commit` sets the signal),
  then TASK-325 lands (hook starts requiring it). Landing them in the
  wrong order locks the repo.
- TASK-326 (other commit-issuing skills) will surface fallout from this —
  any skill that calls `git commit` directly will start failing.
