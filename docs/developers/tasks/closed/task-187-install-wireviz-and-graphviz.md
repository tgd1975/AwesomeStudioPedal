---
id: TASK-187
title: Install WireViz and Graphviz; verify in devcontainer and local dev
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: wiring-as-code
order: 1
---

## Description

Create a `requirements-wiring.txt` file at the repo root pinning `wireviz==0.4` and its
transitive dependency `graphviz` (Python binding). Verify that `wireviz --version` runs
successfully in the local dev environment. Check whether Graphviz (the system package) is
already present in the devcontainer Dockerfile; add it if not.

This is the foundation task for IDEA-019 (Wiring as Code). All subsequent wiring tasks
depend on a reproducible, pinned toolchain.

## Acceptance Criteria

- [ ] `requirements-wiring.txt` exists at repo root with `wireviz==0.4` pinned
- [ ] `wireviz --version` succeeds locally (`pip install -r requirements-wiring.txt`)
- [ ] Graphviz system package is present in `.devcontainer/` config (add if missing)

## Test Plan

No automated tests required — tooling/environment change only.

## Notes

- WireViz requires both the Python package (`pip install wireviz`) and the Graphviz system
  binary (`apt install graphviz` on Debian/Ubuntu). Both are needed.
- Pin to `wireviz==0.4` exactly — the simplified solution (IDEA-019) specifies this version.
- If the devcontainer already has Graphviz, document that in the PR description.
