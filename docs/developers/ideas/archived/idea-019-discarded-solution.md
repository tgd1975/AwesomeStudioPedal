# Preferred Solution — Circuit & Wiring Diagrams as Code

> **DEPRECATED** — This solution was discarded. See the current approach:
> [idea-019-simplified-solution.md](idea-019-simplified-solution.md).

Derived from [concept.md](concept.md). This document captures the chosen stack only; rationale, alternatives, and risks live in the concept.

## 1. Summary

A docs-as-code workflow for electrical schematics and wiring diagrams that lives in Git, renders inline in Markdown, and exports one-way to KiCad for PCB production. Two personas (Engineer, Builder) are served from a single source by generating multiple SVG/BOM artefacts at build time.

## 2. Chosen stack

| Concern | Choice | Notes |
|---|---|---|
| Engineer source (schematics) | **atopile** (`.ato`) | Declarative DSL, units & assertions, KiCad PCB sync. Pin `atopile~=0.6.0`. |
| Engineer fallback | **SKiDL** | Activate only if the atopile spike (§10 of concept) exposes blocking engineering-side issues. |
| Web-stack fallback | **tscircuit** | Activate only if the docs site later becomes a web app needing browser-side interactive rendering. |
| Builder source (harness/wiring) | **WireViz** (YAML) | Cables, connectors, pinouts, color codes, auto-BOM. Pin `wireviz==0.4`. |
| PCB output | **KiCad 9.x** | Forward-only from atopile; back-annotation from PCB edits is not supported (see risk K7). |
| Docs platform | MkDocs or Sphinx | Drives the file-watcher / `make` target for build-time rendering. |
| Inline rendering pattern | §6 pattern 1 — build-time SVG generation | Commit the generated SVG so GitHub web view renders without a build step. |
| Stale-SVG protection | `pre-commit` hook + CI `git diff --exit-code` | Regenerate in CI; fail if working tree is dirty. |
| Editor tooling | Marketplace extensions only (`atopile.atopile`, `bierner.markdown-mermaid`) | No custom VS Code extension. |
| Fritzing exporter | Not built | Deferred behind an explicit gate (concept §12). |

## 3. Authoring & rendering flow

1. Engineer edits `.ato` (schematic) and/or `.yml` (harness) sources alongside code.
2. Local pre-commit hook runs `ato build` and the WireViz CLI; produces SVG + BOM; `git add`s the outputs.
3. Markdown pages reference the generated SVGs and BOM tables — Engineer pages show the schematic view, Builder pages show the WireViz harness + pinout + BOM + free-text assembly notes.
4. `mkdocs serve` / `sphinx-autobuild` provides live browser preview during editing.
5. CI regenerates all artefacts and runs `git diff --exit-code` to block stale-SVG commits.
6. PCB production: `ato build` updates `.kicad_pcb`; KiCad emits Gerbers.

## 4. Persona-to-artefact mapping

| Persona | Page contents | Generated from |
|---|---|---|
| Engineer | Schematic SVG, parameter/tolerance tables, ERC results, BOM | `.ato` via `ato build` |
| Builder | WireViz harness SVG, pinout table, BOM, hand-written assembly notes | `.yml` via `wireviz` CLI |

## 5. Explicit non-goals

- No round-trip / back-annotation from KiCad PCB edits into `.ato` (forward-only; risk K7).
- No textual assembly-instruction view (out of scope; concept §9).
- No custom Markdown fenced-block plugin (concept §6 pattern 2 is aspirational).
- No custom VS Code extension (concept §13).
- No Fritzing importer/exporter (concept §12).

## 6. Gates for revisiting the stack

- **Switch atopile → SKiDL** if the spike (concept §10 step 2) shows atopile cannot express required engineering constructs or its pre-1.0 churn (risk K1) is unacceptable.
- **Add tscircuit** if the docs site moves to a web-app platform and interactive in-page schematics become a real requirement.
- **Build the Fritzing breadboard exporter** only after the Builder persona repeatedly requests a breadboard view and a spike confirms tractable auto-layout (concept §12).
- **Build a VS Code extension** only after observed, repeated complaints that browser-preview latency materially harms the engineer's workflow (concept §13).

## 7. Immediate next steps

1. Add a future-ideas entry to adopt MkDocs (Material theme) if/when docs discoverability becomes a pain point — not needed now; GitHub already renders the Markdown docs adequately.
2. Run the atopile + WireViz spike on the LM7805 + LED + 3-pin connector example (concept §10 steps 2–3).
3. Wire up the pre-commit hook and CI staleness guard (concept §10 step 4).
4. Trial KiCad → atopile migration on one existing project to validate the forward-only path (concept §10 step 5).
