---
id: TASK-199
title: End-to-end validation of the wiring-as-code workflow
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: Support
epic: wiring-as-code
order: 12
prerequisites: [TASK-196, TASK-193]
---

## Description

Perform a full end-to-end validation of the wiring-as-code workflow to confirm that all
moving parts work together: local editing, pre-commit hook, CI staleness guard, and
GitHub rendering. Edit one of the YAML harness files, commit it, push, and verify the
complete pipeline fires correctly.

## Acceptance Criteria

- [ ] Edit a YAML harness file (e.g. change a wire color) and run `git commit`
- [ ] Pre-commit hook fires, regenerates the SVG and BOM, and adds them to the commit
- [ ] Push to a branch; CI staleness guard passes (SVG matches what `wireviz` produces)
- [ ] Rendered SVG on GitHub shows the updated diagram
- [ ] Revert the change and confirm the hook fires again on the revert commit

## Test Plan

No automated tests required — manual end-to-end integration validation.

## Prerequisites

- **TASK-196** — All Fritzing artefacts retired; WireViz is the sole wiring source.
- **TASK-193** — CI staleness guard in place; both local and CI enforcement are active.

## Notes

- This is the sign-off task for the wiring-as-code implementation. If any step fails,
  create a follow-up task to fix the specific issue rather than blocking the whole group.
- `human-in-loop: Support` — a human should review the rendered SVG on GitHub to confirm
  it looks correct, as automated checks cannot validate visual output quality.
- Document the result (pass/fail for each step) in the PR description.
