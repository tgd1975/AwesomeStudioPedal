---
id: TASK-193
title: Add CI staleness guard for WireViz generated outputs
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 7
prerequisites: [TASK-192]
---

## Description

Add a GitHub Actions job (or extend an existing workflow) that installs WireViz and
Graphviz, regenerates all `*.svg` and `*.bom.tsv` files under `docs/builders/wiring/`,
and then runs `git diff --exit-code` to fail the CI build if the working tree is dirty.
This ensures that a PR can never merge with stale generated outputs.

## Acceptance Criteria

- [ ] CI job installs `wireviz==0.4` and Graphviz system package
- [ ] CI regenerates all WireViz outputs under `docs/builders/wiring/`
- [ ] `git diff --exit-code` fails the job if any SVG or BOM differs from what is committed
- [ ] Job runs on pull_request and push to main

## Test Plan

No automated tests required — CI pipeline change only.
Verification: open a PR that has a modified YAML but stale SVG; confirm CI fails with a
clear message. Then commit the regenerated SVG and confirm CI passes.

## Prerequisites

- **TASK-192** — Pre-commit hook in place; CI and pre-commit use the same regeneration
  logic (one is local enforcement, the other is server-side enforcement).

## Notes

- Prefer adding a new job to the existing `docs.yml` workflow if it already runs on PRs;
  otherwise create a minimal `wiring-check.yml`.
- The regeneration command: `wireviz docs/builders/wiring/**/*.yml` (glob) or a loop
  over all YAML files found under that directory.
- Error message on failure should tell the contributor: "SVG/BOM is stale — run
  `wireviz <file>` locally and commit the updated outputs."
