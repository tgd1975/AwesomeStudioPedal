---
id: TASK-245
title: Diagrams — decision flows (B1, B2, B4)
status: closed
closed: 2026-04-25
opened: 2026-04-25
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Main
epic: circuit-skill-diagrams
order: 5
---

## Description

Three Mermaid flowcharts that turn the long branching prose passages into
glanceable decision trees — the CI contract, the overflow response ladder, and
the v0.1 → v1 staging trigger gate. All three currently sprawl across multiple
paragraphs and bullet lists; each compresses to a clean `flowchart TD`.

**B1 — CI contract flow.** Embeds in
[idea-027-layout-engine-concept.md §12](../../ideas/open/idea-027-layout-engine-concept.md).
Source: the existing 6-step CI prose (parse → component-set check → fingerprint
check → hash compute → fast-path or re-render fallback → pass/fail). Type:
`flowchart TD` with explicit FAIL/PASS terminal nodes and the structural-equal
fallback as a side branch.

**B2 — Overflow response ladder.** Embeds in
[idea-027-layout-engine-concept.md §8.3](../../ideas/open/idea-027-layout-engine-concept.md).
Source: the three-rung prose (local grow → neighbour nudge → fail-or-reflow)
plus the cascade-stop rule on rung 2. Type: `flowchart TD`. Make the
"cascade-fails-the-run" edge from rung 2 visually distinct from the success
path, and show that rung 3 is opt-in (no auto edge into it).

**B4 — v0.1 → v1 staging trigger gate.** Embeds in
[idea-027-layout-engine-concept.md §17.1](../../ideas/open/idea-027-layout-engine-concept.md)
and cross-referenced from
[idea-027-circuit-skill.md Phase 2b](../../ideas/open/idea-027-circuit-skill.md).
Source: the post-reframe trigger language ("v1 is gated on concrete
kernel-failure modes accumulating on real use"). Type: `flowchart TD` showing
the release-prep evaluation: kernel-failure observed? → §5.3 row addition
retires it? → open Phase 2b OR stay v0.1.

## Acceptance Criteria

- [ ] Three Mermaid blocks added to the cited sections
- [ ] B1 distinguishes FAIL (block merge) from PASS-with-warn (allow merge,
      flag stale meta) — these are different outcomes in the prose and must be
      different terminal nodes in the diagram
- [ ] B2 shows that rung 2 fails the run if the nudge cascades, and that rung
      3 (`--reflow`) is reachable only via human intervention
- [ ] B4 reflects the maker-community-reframed trigger language (concrete
      kernel-failure on real use), not the pre-reframe §19 triggers
- [ ] `/lint` passes

## Test Plan

`/lint` for syntax. Walk each diagram against the prose decision points: every
numbered step in §12 should be a node in B1; every rung in §8.3 should be a
node in B2; the §17.1 "Trigger for v1" paragraph's logic should map one-to-one
to B4's branches. If a prose decision point is missing from the diagram, the
diagram is incomplete.

## Notes

Draw B1 first — it is the most concrete (CI behaviour is fully specified). B2
second (three rungs, one cascade rule). B4 last because it depends on the
post-reframe wording from EPIC-018's prerequisite review pass.

These three diagrams are visually similar (all `flowchart TD` with diamond
decision nodes); use consistent shapes and colours across them so a reader
recognising B1's pattern can read B2 and B4 quickly.
