---
id: TASK-242
title: Diagrams — static structure (D1, D2, D4)
status: closed
closed: 2026-04-25
opened: 2026-04-25
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Main
epic: circuit-skill-diagrams
order: 2
---

## Description

Three Mermaid diagrams that capture the skill's static structure — the Python
module dependency graph, the NetGraph class model, and the phase plan as a DAG.
These let a new contributor orient themselves without reading the file tree and
phase prose linearly.

**D1 — Skill module dependency graph.** Embeds in
[idea-027-circuit-skill.md §Architecture](../../ideas/open/idea-027-circuit-skill.md),
either replacing or supplementing the file tree. Source: the prose around the file
tree plus the cross-references in skill-packaging.md. Type: `flowchart LR` showing
`layout.py` → `layout_engine/{kernel,router,ai_placer}.py`, `renderer.py` →
`netgraph.py`, `erc_engine.py` → `netgraph.py` + `knowledge/rules.json`,
`bom_exporter.py` → `components` (no NetGraph), `netlist_exporter.py` →
`netgraph.py`. Mark `ai_placer.py` as v1-only.

**D2 — NetGraph class model.** Embeds in
[idea-027-erc-engine.md §Net graph data model](../../ideas/open/idea-027-erc-engine.md)
just below the existing dataclass snippet. Source: the dataclass + helpers prose.
Type: `classDiagram` with `NetGraph`, `PinRef`, `NetMeta`, the methods
(`pins_on_net`, `nets_containing_pin`, `flattened_segments`, `components_between`)
and which check consumes which (annotated as notes).

**D4 — Phase plan dependency DAG.** Embeds in
[idea-027-circuit-skill.md §Phase Plan](../../ideas/open/idea-027-circuit-skill.md).
Source: the prose phase order (Phase 1 → 2a → {3, 4, 5, 6}; Phase 2b contingent;
Phase 7 needs 6 + real use). Type: `flowchart LR` with arrows for hard
prerequisites, dashed arrows for soft / contingent dependencies (Phase 2b).

## Acceptance Criteria

- [ ] Three Mermaid blocks added to the cited sections; each is a strict addition
      that does not contradict surrounding prose
- [ ] `D1` correctly marks `ai_placer.py` as absent in v0.1
- [ ] `D2` distinguishes ERC consumers from netlist consumers per the doc's
      "Helper semantics" list
- [ ] `D4` distinguishes hard prerequisites from contingent edges visually
- [ ] `/lint` passes

## Test Plan

`/lint` for syntax. Eyeball each diagram in the IDE preview: D1's arrows should
match the imports declared in the prose, D2's class contents should match the
dataclass code block one-for-one, D4's edges should match the "hard prerequisite"
sentences in §Phase Plan.

## Notes

D1 may overlap visually with the file tree — keep the tree, add the diagram below
it as the relationship view. The tree shows *where files live*; the diagram shows
*what depends on what*.

D2 is the most rigid of the three (class structure must match code). D4 is the
most flexible (judgement on which arrows to draw — keep it readable, not
exhaustive).
