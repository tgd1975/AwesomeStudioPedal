---
id: TASK-204
title: Update CI staleness guard — WireViz → Schemdraw
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: None
epic: circuit_diagram_as_code
order: 5
prerequisites: [TASK-203]
---

## Description

Update `.github/workflows/docs.yml` to replace the WireViz staleness check with a
Schemdraw staleness check. The CI job must regenerate both SVGs and fail if the committed
versions differ from the freshly generated ones.

Current job to replace (`wiring-check`):

```yaml
- name: Install Graphviz and WireViz
  run: |
    sudo apt-get install -y -q graphviz
    pip install -r requirements-wiring.txt

- name: Regenerate WireViz outputs
  run: |
    find docs/builders/wiring -name '*.yml' | while read yml; do
      wireviz -f st "$yml"
    done

- name: Fail if any generated file is stale
  run: |
    if ! git diff --exit-code docs/builders/wiring/; then
      echo "SVG or BOM is stale — run 'wireviz -f st <file>.yml' locally ..."
      exit 1
    fi
```

## Acceptance Criteria

- [ ] `wiring-check` job in `docs.yml` renamed to `schematic-check` (or equivalent)
- [ ] WireViz install step replaced with `pip install -r requirements-schematic.txt`
- [ ] Regeneration step runs:
      `python scripts/generate-schematic.py --target esp32`
      `python scripts/generate-schematic.py --target nrf52840`
- [ ] Staleness check runs `git diff --exit-code` on both SVG files
- [ ] Failure message names the stale file and tells the developer exactly what command to
      run locally to fix it
- [ ] CI passes on the current branch after this change

## Test Plan

Push a branch where `data/config.json` has been changed but SVGs have not been
regenerated — CI must fail. Then regenerate and push — CI must pass.

## Documentation

- `.github/workflows/docs.yml` — updated
