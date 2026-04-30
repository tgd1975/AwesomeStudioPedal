---
id: TASK-330
title: Decide content-page open questions (source-of-truth, i18n, context-awareness, first-run)
status: open
opened: 2026-04-30
effort: Small (<2h)
complexity: Medium
human-in-loop: Main
epic: app-content-pages
order: 1
---

## Description

IDEA-037 leaves four design questions unresolved. Resolve them before any
UI is built so all subsequent content-page tasks share the same
foundation.

The four questions:

1. **Source of truth for content** — hand-write per page in Dart strings,
   or pull from existing developer docs (`docs/users/...`) so we don't
   maintain two copies.
2. **Localization** — English only for now, or scaffold `flutter_intl` /
   `flutter_localizations` from day one.
3. **How-To context-awareness** — static walkthrough, or context-aware
   (show "pair your pedal" only while no pedal is connected).
4. **First-run flow** — auto-show How-To on first launch, or purely
   opt-in via the menu.

Output: a short decision document under
`docs/developers/decisions/` (or appended to ARCHITECTURE.md if that's
the convention) capturing the decision and one-line rationale per
question. Subsequent tasks (TASK-331..TASK-338) reference this.

## Acceptance Criteria

- [ ] All four questions answered with a one-line rationale each.
- [ ] Decision document committed (or rationale appended to existing
      design doc) and linked from this task.
- [ ] If localization is "scaffold from day one", a follow-up scaffolding
      step is named (in this task or as a new prerequisite for TASK-331).

## Test Plan

No automated tests required — change is non-functional.

## Notes

- This is a decision task, not an implementation task. Keep it small.
- The user is the decision-maker; agent's role is to surface trade-offs
  and write up the chosen path.
