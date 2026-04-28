---
id: TASK-200
title: Install Schemdraw and verify toolchain
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: None
epic: circuit_diagram_as_code
order: 1
prerequisites: []
---

## Description

Install Schemdraw and Matplotlib and verify they run correctly in both the local dev
environment and the devcontainer. Schemdraw is a Python circuit-drawing library that
produces documentation-quality SVGs from Python source — the right tool for the Builder's
circuit schematic view.

Note: atopile was evaluated as a candidate tool but produces KiCad PCB layout files and
requires `kicad-cli` — it is not a standalone SVG renderer. It was not pinned or added
to the devcontainer.

## Acceptance Criteria

- [ ] `python -c "import schemdraw; print(schemdraw.__version__)"` runs successfully locally
- [ ] `schemdraw` and `matplotlib` added to `requirements-schematic.txt` with pinned
      minor versions (e.g. `schemdraw~=0.19`, `matplotlib~=3.9`)
- [ ] `requirements-wiring.txt` removed or cleared — WireViz is being replaced
- [ ] devcontainer updated to install from `requirements-schematic.txt`
- [ ] `/devcontainer-sync` run to persist the change

## Test Plan

```bash
python -c "import schemdraw, matplotlib; print('OK')"
```

## Notes

Schemdraw is already used in the proof-of-concept at
`docs/developers/ideas/archive/idea-019-esp32-wiring.py`. This task just pins it properly
and wires it into the toolchain. Pin tightly — use `~=` not `>=`.

Check current releases: `pip index versions schemdraw`.

## Documentation

- `requirements-schematic.txt` — new file (or rename/replace `requirements-wiring.txt`)
- `.devcontainer/devcontainer.json` — update install step
