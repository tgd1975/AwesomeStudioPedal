---
id: TASK-203
title: Replace pre-commit hook — WireViz → Schemdraw
status: closed
closed: 2026-04-22
opened: 2026-04-22
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: None
epic: circuit_diagram_as_code
order: 4
prerequisites: [TASK-201, TASK-202]
---

## Description

Update `scripts/pre-commit` to regenerate circuit SVGs via
`python scripts/generate-schematic.py` instead of `wireviz`. The hook must run the
schematic generator and `git add` the resulting SVGs when source files that affect the
diagrams are staged.

Trigger files (regenerate both SVGs when any of these are staged):

- `scripts/generate-schematic.py` — the drawing script itself
- `data/config.json` — ESP32 GPIO assignments
- `lib/hardware/esp32/include/builder_config.h`
- `lib/hardware/nrf52840/include/builder_config.h`

Current behaviour to replace:

```bash
# scripts/pre-commit — current WireViz block
WIREVIZ=$(command -v wireviz 2>/dev/null || true)
wireviz -f st "$yml"
```

## Acceptance Criteria

- [ ] WireViz invocation removed from `scripts/pre-commit`
- [ ] Hook detects staged trigger files (see above) and runs
      `python scripts/generate-schematic.py --target esp32` and `--target nrf52840`
- [ ] Hook `git add`s both regenerated SVGs after a successful run
- [ ] Hook prints a clear warning (not a hard failure) when `schemdraw` is not importable,
      matching the existing WireViz degraded-mode pattern
- [ ] Committing a change to `data/config.json` triggers regeneration and stages updated SVGs
- [ ] Committing an unrelated file does not trigger regeneration

## Test Plan

1. Modify a GPIO value in `data/config.json`.
2. `git add data/config.json` only.
3. `git commit` — hook must regenerate and stage both SVGs automatically.
4. Confirm the commit includes `data/config.json` and both `main-circuit.svg` files.
5. Repeat with an unrelated file change — confirm hook does not run.

## Documentation

- `scripts/pre-commit` — updated
