---
id: TASK-244
title: Diagram — contributor workflow sequence (C1)
status: closed
closed: 2026-04-25
opened: 2026-04-25
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: Clarification
epic: circuit-skill-diagrams
order: 4
---

## Description

A Mermaid sequence diagram showing the canonical contributor workflow — a maker
edits `.circuit.yml`, runs `/circuit layout <name>`, the kernel attempts
deterministic placement, optionally the AI placer is invoked (v1 only),
artefacts are written, and the maker commits. Embeds in
[idea-027-layout-engine-concept.md §13](../../ideas/open/idea-027-layout-engine-concept.md).

This is its own task — different from the dataflow batch — because it is a
sequence diagram, not a flowchart, and addresses the human-as-actor view that
the static diagrams don't cover.

Source: the §13 numbered workflow plus the §7 AI placer convergence loop. Type:
`sequenceDiagram` with three lanes — `User`, `Skill (kernel)`, `AI placer (v1)` —
plus an optional fourth `Git` lane for the commit step. Show the v0.1 / v1
divergence as a Mermaid `alt` block (kernel-only branch vs. kernel + AI branch).

## Acceptance Criteria

- [ ] Mermaid `sequenceDiagram` block embedded in §13
- [ ] v0.1 (kernel-only) and v1 (kernel + AI) paths both visible in one
      diagram via an `alt` / `else` block, not duplicated as two diagrams
- [ ] AI placer's "iteration cap 5" and the §7.3 failure path both surface in
      the v1 branch (loop + escalation message)
- [ ] `/lint` passes

## Test Plan

`/lint` for syntax. Render in IDE preview and walk the message ordering against
§13's numbered list — every numbered step in the prose should map to one or
more arrows in the diagram, and the AI iteration loop should match the §7.3
cap-and-escalate contract.

## Notes

Keep the sequence under ~12 messages — sequence diagrams stop being readable
beyond that. If it grows, split out the AI convergence subloop into a separate
diagram (which becomes the C2 candidate from the analysis, deferred to a later
task).

The `--no-ai` flag's behaviour differs between v0.1 (no-op) and v1
(load-bearing) per skill-packaging.md item 5 — surface this in the diagram only
if it stays compact; otherwise leave the divergence to the §13 prose.
