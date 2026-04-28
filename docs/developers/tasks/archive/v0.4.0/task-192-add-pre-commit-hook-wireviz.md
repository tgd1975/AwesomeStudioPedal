---
id: TASK-192
title: Add pre-commit hook to regenerate WireViz outputs on YAML changes
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 5
prerequisites: [TASK-189]
---

## Description

Add a `.pre-commit-config.yaml` entry (or extend the existing `scripts/pre-commit` hook)
that runs `wireviz` on any staged `*.yml` files under `docs/builders/wiring/` and
`git add`s the generated `*.svg` and `*.bom.tsv` outputs. This prevents stale diagrams
from being committed when only the YAML source is staged.

## Acceptance Criteria

- [ ] Pre-commit hook runs `wireviz` on staged YAML files under `docs/builders/wiring/`
- [ ] Generated `*.svg` and `*.bom.tsv` are automatically staged by the hook
- [ ] Editing a YAML and running `git commit` triggers the hook and re-adds the outputs
- [ ] Hook does not interfere with commits that contain no wiring YAML changes

## Test Plan

No automated tests required — CI/tooling change only.
Manual verification: edit `main-harness.yml`, `git add` it, run `git commit`, and confirm
the hook fires and stages the updated SVG.

## Prerequisites

- **TASK-189** — `main-harness.svg` and `main-harness.bom.tsv` exist; hook has real files
  to regenerate for a smoke test.

## Notes

- Check if a `.pre-commit-config.yaml` already exists at the repo root before creating one.
- If the project uses a custom `scripts/pre-commit` shell script, integrate there instead
  of adding a separate `.pre-commit-config.yaml`.
- The hook should be scoped to `docs/builders/wiring/**/*.yml` — do not run `wireviz` on
  unrelated YAML files (e.g. GitHub Actions workflows).
- The hook must be idempotent: re-running on an already-up-to-date SVG should be a no-op.
