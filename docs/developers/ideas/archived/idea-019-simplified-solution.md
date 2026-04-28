# Simplified Solution — WireViz → Schemdraw

> **SUPERSEDED** — The WireViz harness approach (TASK-187–199) was replaced by a Schemdraw
> circuit schematic approach (TASK-200–207). WireViz produced wiring-harness views, not the
> circuit schematics the idea originally called for.
>
> **Current implementation:** `scripts/generate-schematic.py` draws documentation-quality
> circuit schematics (MCU label, GPIO pins, 220 Ω resistors, LEDs, push buttons, GND rails)
> for both the ESP32 and nRF52840 targets. Output SVGs are committed to
> `docs/builders/wiring/<target>/main-circuit.svg` and regenerated automatically by a
> pre-commit hook and CI staleness guard. Implemented: 2026-04-22 via TASK-200–TASK-207.
>
> **What was intentionally left out:** BOM generation, PCB export — deferred to IDEA-011.
>
> **Prior approaches evaluated and rejected:**
>
> - WireViz (TASK-187–199): correct tooling for wiring harnesses, wrong diagram type for
>   circuit schematics.
> - atopile (TASK-200 spike): produces KiCad PCB layout files, requires `kicad-cli`.
>   Not a standalone SVG renderer.
>
> **Relationship to IDEA-011:** Schemdraw is the documentation layer. When PCB design starts
> (IDEA-011), KiCad + KiBot become the source of truth and replace the Schemdraw script.

---

*The original WireViz solution notes are preserved below for historical reference.*

A reduced version of [preferred-solution.md](preferred-solution.md) that drops atopile, KiCad, and the Engineer-vs-Builder split. One DSL, one generator, one persona.

## 1. Scope

- **In scope:** wiring harnesses, pinouts, cable BOMs — anything WireViz can express.
- **Out of scope:** schematic capture, ERC, PCB layout, Gerber export. If a PCB is needed, it is designed elsewhere (e.g. KiCad GUI) and only its connectors appear in WireViz.
- **Status of the committed YAML/SVG:** a *reference wiring example* matching each target's default config header. It is **not** a source of truth for pin assignments — the firmware's per-target `builder_config.h` remains canonical. Builders who change pin assignments edit their local config; they are not expected to keep the reference YAML in sync. Each rendered page carries a banner (e.g. *"Reference wiring for the default config. Your build may differ."*).

## 2. Stack

| Concern | Choice | Notes |
|---|---|---|
| Source DSL | **WireViz** YAML | Pin `wireviz==0.4`. |
| Renderer | WireViz CLI (Python + Graphviz) | Emits SVG + TSV BOM. |
| Docs platform | GitHub native Markdown rendering | MkDocs deferred — see §7.1. |
| Inline rendering | Build-time SVG, committed to Git | GitHub web view renders without a build step. |
| Stale-SVG protection | `pre-commit` hook + CI `git diff --exit-code` | Regenerate in CI; fail if working tree is dirty. |
| Editor tooling | YAML schema validation (built into VS Code) | No custom extension. |

## 3. Workflow

1. Author edits `*.yml` under `docs/builders/wiring/<target>/`.
2. Pre-commit hook runs `wireviz` on changed YAMLs; produces `*.svg` + `*.bom.tsv`; `git add`s the outputs.
3. Markdown pages reference the generated SVG (e.g. `![ESP32 reference wiring](wiring/esp32/main-harness.svg)`) and embed/link the BOM.
4. Local preview: VS Code Markdown preview is sufficient; no build server required.
5. CI regenerates and runs `git diff --exit-code` to block stale-SVG commits.

```
   *.yml ── wireviz ──► *.svg
                        *.bom.tsv  ──► Markdown page
```

## 4. Repository layout

One reference harness per hardware target, colocated with the Builder docs:

```
docs/builders/wiring/
  esp32/
    main-harness.yml
    main-harness.svg        # generated, committed
    main-harness.bom.tsv    # generated, committed
  nrf52840/
    main-harness.yml
    main-harness.svg
    main-harness.bom.tsv
.pre-commit-config.yaml
```

## 5. Explicit non-goals

- No schematic DSL, no atopile, no SKiDL, no tscircuit.
- No PCB output, no KiCad integration, no Gerbers.
- No two-persona split — one set of pages for everyone.
- No custom Markdown plugin, no custom VS Code extension, no Fritzing exporter. Existing Fritzing artefacts are phased out and replaced by WireViz SVGs on a per-target basis (replace-then-remove, so docs are never without a wiring visual).
- No code-generation between `builder_config.h` and the YAML, and no CI drift check between them — the YAML is a reference example, not a mirror of the config.
- No cross-tool sync glue (there is only one tool).

## 6. Gates for revisiting

- **Add atopile (or SKiDL)** if a real schematic source-of-truth becomes necessary — i.e. when "the schematic lives in a GUI and drifts from the wiring docs" becomes a recurring pain point. At that point return to [preferred-solution.md](preferred-solution.md).
- **Add tscircuit** only if the docs site moves to a web-app stack needing interactive in-page diagrams.

## 7. Immediate next steps

1. Add a future-ideas entry to adopt MkDocs (Material theme) if/when docs discoverability becomes a pain point — not needed now; GitHub already renders the Markdown docs adequately.
2. Pilot on ESP32: author `docs/builders/wiring/esp32/main-harness.yml` matching the current default `builder_config.h`; commit the generated SVG + BOM; replace the corresponding Fritzing PNG in `docs/media/` with a link to the new SVG.
3. Add the `pre-commit` hook (`wireviz` + `git add`) and the CI staleness guard.
4. Once the ESP32 pilot is stable, repeat for nRF52840 and retire the remaining Fritzing artefacts.

## 8. Task plan

Phases are sequential at their boundaries; tasks within a phase can run in parallel where noted. Task IDs use a `W` prefix (Wiring) to avoid collisions with the project's existing `TASK-nnn` numbering — remap when creating actual task files.

### Phase A — Tooling & environment

| ID | Title | Effort | Prerequisites | Notes |
|----|-------|--------|---------------|-------|
| W-01 | Install WireViz and Graphviz; verify `wireviz --version` works in devcontainer and local dev | Small (<2h) | — | Pin `wireviz==0.4` in a `requirements-wiring.txt`. Add Graphviz to devcontainer Dockerfile if not present. |

### Phase B — ESP32 pilot harness

| ID | Title | Effort | Prerequisites | Notes |
|----|-------|--------|---------------|-------|
| W-02 | Author `docs/builders/wiring/esp32/main-harness.yml` | Small (1-2h) | W-01 | Match default `builder_config.h` GPIO assignments (buttons, select LEDs, power LED). Include connector types, wire gauges, color codes. |
| W-03 | Generate SVG + BOM; commit both | Small (<1h) | W-02 | Run `wireviz` manually once; commit `main-harness.svg` + `main-harness.bom.tsv`. Verify SVG renders on GitHub. |
| W-04 | Embed SVG in `BUILD_GUIDE.md` and add "reference wiring" banner | Small (<1h) | W-03 | `![ESP32 reference wiring](../wiring/esp32/main-harness.svg)` + italicised banner: *"Reference wiring for the default config. Your build may differ."* |
| W-05 | Retire ESP32 Fritzing artefacts from `docs/media/` | Small (<1h) | W-04 | Remove `AwesomeStudioPedal_esp32_wiring.fzz`, `*_breadboard.png`, `*_circuit.png`, `*_pcb.png`. Update any remaining links. |

### Phase C — Pre-commit hook & CI

| ID | Title | Effort | Prerequisites | Notes |
|----|-------|--------|---------------|-------|
| W-06 | Add `.pre-commit-config.yaml` entry for WireViz | Small (1-2h) | W-03 | Hook runs `wireviz` on staged `*.yml` under `docs/builders/wiring/`; `git add`s generated outputs. Integrate with existing `scripts/pre-commit` if present. |
| W-07 | Add CI staleness guard (GitHub Actions) | Small (1-2h) | W-06 | Job: checkout → install wireviz+graphviz → regenerate → `git diff --exit-code`. Fail if SVG/BOM differ. |

### Phase D — nRF52840 rollout

| ID | Title | Effort | Prerequisites | Notes |
|----|-------|--------|---------------|-------|
| W-08 | Author `docs/builders/wiring/nrf52840/main-harness.yml` | Small (1-2h) | W-07 | Match default nRF52840 `builder_config.h`. Same structure as W-02. |
| W-09 | Generate, commit, embed, add banner (nRF52840) | Small (<1h) | W-08 | Mirror W-03 + W-04 for nRF52840 target. |
| W-10 | Retire any remaining Fritzing artefacts | Small (<1h) | W-09 | Final cleanup of `docs/media/` Fritzing files if any remain. |

### Phase E — Documentation & idea tracking

| ID | Title | Effort | Prerequisites | Notes |
|----|-------|--------|---------------|-------|
| W-11 | Update IDEA-019 (wiring-as-code) — mark as implemented / link to this solution | Small (<1h) | W-09 | Close or annotate `idea-019-wiring-as-code.md`. |
| W-12 | Create IDEA for MkDocs adoption | Small (<1h) | — | New `idea-022-mkdocs-adoption.md` per §7.1. Can run in parallel with any phase. |

### Phase F — Validation

| ID | Title | Effort | Prerequisites | Notes |
|----|-------|--------|---------------|-------|
| W-13 | End-to-end validation | Small (1-2h) | W-10, W-07 | Edit a YAML → commit → verify pre-commit regenerates → push → verify CI passes. Review rendered SVG on GitHub. |

### Dependency graph

```
W-01 ──► W-02 ──► W-03 ──► W-04 ──► W-05
                    │
                    ▼
                  W-06 ──► W-07 ──► W-08 ──► W-09 ──► W-10 ──► W-13
                                                        │
                                                        ▼
                                                      W-11

W-12 (independent, any time)
```

### Totals

- **13 tasks**, all Small effort
- **Critical path:** W-01 → W-02 → W-03 → W-06 → W-07 → W-08 → W-09 → W-10 → W-13 (9 tasks)
- **No blockers on existing open tasks** — fully independent of the 23 open tasks in the project
