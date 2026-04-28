---
id: TASK-241
title: Diagrams — pipeline and dataflow (A1, A2, A3)
status: closed
closed: 2026-04-25
opened: 2026-04-25
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Main
epic: circuit-skill-diagrams
order: 1
---

## Description

Three Mermaid diagrams that make the skill's data spine visible at a glance — the
end-to-end pipeline, the shared NetGraph substrate, and the YAML-form to KiCad-net
mapping. These are the highest-leverage diagrams in the set: they replace ~50 lines
of prose pipeline-as-text plus several cross-doc references with one picture each.

**A1 — End-to-end pipeline.** Embeds in
[idea-027-circuit-skill.md §Pipeline](../../ideas/open/idea-027-circuit-skill.md).
Source: the existing ASCII pipeline (`.circuit.yml` → schema validate → ERC →
layout kernel → router → render → rubric + meta.yml) plus the parallel BOM and
netlist branches that fork off NetGraph. Type: `flowchart LR` with subgraphs for
the kernel/render/exporter clusters.

**A2 — NetGraph as shared substrate.** Embeds in
[idea-027-erc-engine.md §Net graph data model](../../ideas/open/idea-027-erc-engine.md).
Source: the prose "Both `erc_engine.py` and `netlist_exporter.py` consume the
parsed YAML net graph…" and the helper-semantics list. Type: `flowchart` showing
yaml parse → `NetGraph.nets` + `NetMeta` → ERC checks (read both), netlist exporter
(reads both), BOM exporter (does *not* — walks `components` directly).

**A3 — Form-to-output mapping.** Embeds in
[idea-027-exporters.md §Net-shape mapping](../../ideas/open/idea-027-exporters.md).
Source: the existing 3-row table mapping `pins` / `path` / `bus` to `NetGraph.nets`
to KiCad output. Type: `flowchart` with three lanes (one per form), showing the
collapse/split behaviour explicitly — bus collapses to one net, path splits into
N−1 segments, pins stays as one block.

## Acceptance Criteria

- [ ] Three Mermaid blocks added to the cited sections, each replacing or
      augmenting the existing prose without removing accuracy
- [ ] `/lint` passes (markdownlint and Mermaid-syntax checks)
- [ ] Each diagram visibly renders in GitHub's Markdown preview without overflow
      or unreadable label collisions
- [ ] Cross-doc anchors and surrounding prose still read correctly after insertion

## Test Plan

Run `/lint` to validate Markdown and Mermaid syntax. Render each diagram with the
local Mermaid CLI (or in the IDE preview) and eyeball: arrow directions match the
pipeline order, subgraph groupings match the doc's section headings, no orphan
nodes. Re-read the surrounding paragraphs to confirm the diagram makes them
shorter, not redundant.

## Notes

These three are the spine of the design. Draw A1 first — it orients the reader on
the whole skill — then A2 (which A1 references), then A3 (which depends on A2's
NetGraph being established). Use `flowchart LR` for all three for visual
consistency.

Mermaid is the project default per the existing lint setup; no new tooling needed.
