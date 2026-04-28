---
id: TASK-206
title: Remove WireViz artefacts and tooling
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: None
epic: circuit_diagram_as_code
order: 7
prerequisites: [TASK-203, TASK-204, TASK-205]
---

## Description

Delete all WireViz source files, generated artefacts, and tooling from the repo. Run only
after TASK-203 (hook), TASK-204 (CI), and TASK-205 (docs) are all merged and CI is green.

Note on atopile: it was installed locally as a spike during TASK-200 but was rejected
because it requires KiCad and produces PCB layout files, not SVGs. It was never pinned
to a requirements file or added to the devcontainer, so there is nothing to clean up in
the repo for atopile.

## Acceptance Criteria

- [ ] Deleted: `docs/builders/wiring/esp32/main-harness.yml`
- [ ] Deleted: `docs/builders/wiring/esp32/main-harness.svg`
- [ ] Deleted: `docs/builders/wiring/esp32/main-harness.bom.tsv`
- [ ] Deleted: `docs/builders/wiring/nrf52840/main-harness.yml`
- [ ] Deleted: `docs/builders/wiring/nrf52840/main-harness.svg`
- [ ] Deleted: `docs/builders/wiring/nrf52840/main-harness.bom.tsv`
- [ ] `requirements-wiring.txt` deleted (replaced by `requirements-schematic.txt`)
- [ ] devcontainer updated to remove `requirements-wiring.txt` install step,
      if still referenced — `/devcontainer-sync` run
- [ ] No remaining references to `wireviz`, `main-harness.yml`, or `main-harness.svg`
      anywhere in the repo
- [ ] CI passes after deletion

## Test Plan

```bash
grep -r "wireviz" . --include="*.yml" --include="*.md" --include="*.json" --include="*.sh"
grep -r "main-harness" . --include="*.yml" --include="*.md" --include="*.json"
grep -r "requirements-wiring" .
# all must return no results (outside this task file)
```

## Documentation

- `requirements-wiring.txt` — deleted
- `.devcontainer/devcontainer.json` — updated if needed
