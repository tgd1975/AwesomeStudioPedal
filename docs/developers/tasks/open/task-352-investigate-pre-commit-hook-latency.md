---
id: TASK-352
title: Investigate pre-commit hook latency — reorganize, parallelize, or skip irrelevant checks
status: open
opened: 2026-05-01
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Every commit on this repo pays the full pre-commit bill regardless of
what it actually changes. A markdown-only frontmatter flip — e.g.
closing a task, which only touches `*.md` — still runs:

- markdownlint over staged + already-tracked `*.md`
- JSON schema validation
- task-system mirror-sync check
- Mermaid diagram validation in staged markdown
- clang-format check (skipped on no-op, but invoked)
- clang-tidy gate (skipped on no-op, but invoked)
- host-test build/run gate (skipped on no-op, but invoked)
- devcontainer.json well-formedness check
- secret scan
- `housekeep.py --apply` regenerating OVERVIEW / EPICS / KANBAN /
  ideas + four per-release archive overviews
- post-housekeep markdownlint re-run over the regenerated index files

Wall-clock for a docs-only commit is noticeably long. The chain is
**deliberately thorough** — every check earns its keep on commits
that touch the relevant files — but it currently has no notion of
"this commit can't possibly affect that check, so skip it".

Three axes worth examining (not pre-commitments — the task is to
measure, then choose):

1. **Per-file-type gating.** Skip clang-format / clang-tidy / host
   tests when no `*.cpp`/`*.h` is staged. Skip Mermaid validation
   when no `*.md` is staged. The "skipped on no-op" branches already
   exist in some checks — generalise, surface as a fast pre-flight.
2. **Parallelization.** Independent checks (markdownlint, JSON
   schema, secret scan, devcontainer) could run concurrently. The
   sequential ordering today is convenient, not required.
3. **Phase split.** Fast checks (lint, format, sync) run pre-commit;
   slow checks (host tests, clang-tidy on full file set) move to
   pre-push or CI-only, with an explicit fall-through for local
   "I want the full suite" runs.

The output of this task is **not** an implementation. It is a measured
breakdown of where the time goes plus a recommendation on which axis
(or combination) to pursue, written up so a follow-up implementation
task can scope cleanly.

## Acceptance Criteria

- [ ] A timing profile of the pre-commit hook for at least three commit
      shapes: docs-only, C++-only, mixed. Each phase named and timed.
- [ ] A short written analysis (in the task body or a linked doc)
      identifying the top 2–3 latency contributors and proposing a
      preferred direction (per-file gating, parallelization, phase
      split, or a combination), with rationale.
- [ ] A follow-up implementation task (or tasks) scaffolded with
      `/ts-task-new` capturing the chosen direction, sized
      appropriately. This task itself does not implement.

## Test Plan

No automated tests required — change is investigative. The follow-up
implementation task(s) will define their own test plan.

Manual verification: re-run the timing profile after the
implementation task lands and confirm the headline figure improved.

## Notes

- The hook is at `scripts/pre-commit` (or whatever the repo wires
  into `.git/hooks/pre-commit`). Start there for the timing harness —
  wrapping each phase with `time` / `date +%s%N` is enough.
- Don't conflate this with `--no-verify` policy. The goal is a
  *faster correct* hook, not bypassing it.
- The CLAUDE.md "Pre-commit hook failures on unrelated changes"
  protocol is orthogonal — it handles the case where a hook
  *legitimately* fails on someone else's code. Speed and bypass
  policy are independent levers.
- Parallelization caveat: some checks share state (e.g. housekeep
  edits files that the post-housekeep re-lint then reads).
  Dependency graph the chain before parallelizing — naive `&` will
  produce flaky failures.
- Per-file-type gating is the lowest-risk, highest-value lever
  based on the current chain shape, but confirm with measurements
  before committing to that direction.
