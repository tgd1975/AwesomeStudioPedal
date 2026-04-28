---
id: TASK-268
title: Single source of truth for awesome-task-system/ — sync script + divergence guard
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Large (8-24h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Clarification
epic: task-system-rollout
order: 1
---

## Description

Make `awesome-task-system/` the canonical source for the task-system scripts,
skills, config, and tests. The live project copies (`scripts/`, `.claude/skills/`,
`docs/developers/task-system.yaml`) are currently ahead of the package on a
handful of files, and every future change has to be applied twice unless this
is fixed first. With the package destined for carve-out (IDEA-031), the right
move is to flip the model now and avoid paying the doubling cost across the
three other tasks in EPIC-020.

The flow lands as: **edit in the package → run a sync script → live copies
catch up → pre-commit guard fails any commit where the two sides drift.**

Approach (from IDEA-043 §2):

1. Bring the package up to date (one-shot, mechanical copy of the diverged pairs).
2. Add `scripts/sync_task_system.py` — package → live, idempotent, refuses to
   overwrite a dirty live copy without `--force`.
3. Run the sync script once to bring live back in lockstep.
4. Extend `scripts/pre-commit` with a divergence guard that diffs every
   mirrored pair and fails the commit if any differ, naming the file and the
   sync command in the failure message.
5. Update `awesome-task-system/LAYOUT.md`, `awesome-task-system/TASK_SYSTEM.md`,
   and the project root `CLAUDE.md` with the new edit-and-sync workflow.

Bundles **§9.2 of IDEA-043** — housekeeping should validate per-epic `order:`
fields (no blanks, no duplicates, no `?` placeholders), with an opt-in
`--fix-order` mode that renumbers contiguously from 1 within each epic.
Lands here because the package is already being touched and the validator
is a natural addition to the housekeeping surface getting consolidated.

## Acceptance Criteria

- [x] All mirrored files are byte-identical between live and package (`diff -rq` clean).
- [x] `scripts/sync_task_system.py` exists, is idempotent, copies package → live only, and refuses to clobber a dirty live copy without `--force`.
- [x] Divergence guard step in `scripts/pre-commit` fails on a deliberate divergence (verified by editing one live copy and attempting a commit).
- [x] Mirror set covers: the 2 diverged scripts (`housekeep.py`, `update_task_overview.py`), the 5 diverged skill `SKILL.md` files (`ts-task-pause`, `ts-task-active`, `ts-task-new`, `ts-epic-new`, plus `tasks/SKILL.md` newly added to the package), the already-byte-identical scripts and skills (so they stay that way), `config/task-system.yaml`, and `scripts/tests/`.
- [x] `tasks/SKILL.md` added to the package (per §8 Q2 — yes, include).
- [x] `scripts/tests/` mirrored into `awesome-task-system/scripts/tests/` and included in the divergence guard (per §8 Q6 — mirror).
- [x] `awesome-task-system/VERSION` bumped to `0.2.0`. `awesome-task-system/CHANGELOG.md` notes the consolidation (create if absent).
- [x] `awesome-task-system/LAYOUT.md`, `awesome-task-system/TASK_SYSTEM.md`, and the project root `CLAUDE.md` document the edit-and-sync workflow.
- [x] `housekeep.py` validates `order:` fields per epic — no blanks, no duplicates, no `?` placeholders. Failure is loud (names the offending epic and tasks). `--fix-order` flag renumbers contiguously from 1 within each epic.

## Test Plan

**Host tests** (`make test-host` and the existing `scripts/tests/test_housekeep.py` runner):

- Extend `scripts/tests/test_housekeep.py` (mirrored into the package) with cases for the new `order:` validator:
  - missing `order:` on a task with `epic:` set → fail
  - duplicate `order:` within a single epic → fail (names both tasks)
  - `order: ?` → fail (treated as missing)
  - happy path with contiguous orders → pass
  - `--fix-order` against a damaged epic produces contiguous 1..N
- Add a small test for `scripts/sync_task_system.py`:
  - clean state → "already in sync" exit 0
  - intentionally drift one mirrored file → script restores it → exit 0
  - dirty live copy without `--force` → refuses, exits non-zero
- The divergence guard itself is integration-only (it runs as part of pre-commit). Verify manually per the AC.

No on-device tests — change is pure tooling.

## Notes

- **Recommendation taken from §8:** sync script over symlinks (Windows + Ubuntu dual-OS, fragile across git clients).
- **Sync direction is one-way: package → live.** Editing the live copy and forgetting to sync back is exactly the failure mode the divergence guard catches at commit time.
- **Pre-existing tooling:** do not replace `scripts/pre-commit` — extend it. The divergence guard is one more shell-out alongside the existing clang-format / tests checks.
- **Mirror-set scope (per §8 Q4):** `update_idea_overview.py` and the three `ts-idea-*` skills are byte-identical today and need no copy, but **must** be in the divergence-guard scope so they stay that way.
- **Risk: sync script clobbers an in-flight live edit.** Mitigated by the dirty-check + `--force` opt-in.
- **Risk: future skills get added to one side only.** Mitigated by the guard treating "exists in live but not package" (or vice versa) as a divergence, except for the explicit allowlist of project-only files.
- Effort estimate **L (8–24h)**, on the lower end. Mechanical copy is small; most time goes into guard plumbing (path handling, project-only allowlist, integration with pre-commit, fail-mode test) plus docs across 3+ files plus the §9.2 order validator.
