---
id: TASK-148
title: Reorganise Developer Documentation
status: open
opened: 2026-04-18
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
prerequisites: [TASK-147]
---

## Description

Audit and restructure developer-facing documentation so it lives in one coherent place.
Currently app developer docs are split across `app/README.md`, `app/docs/`, and
`docs/developers/` — a new contributor has no single entry point. Additionally,
`docs/building.md` is an orphan with unclear placement.

The task starts with a structural decision (where does app dev content live?) before
executing any moves, to avoid churn.

## Acceptance Criteria

- [ ] Decision documented and agreed: app-specific dev guides stay in `app/docs/` (close
  to the code) OR move to `docs/developers/app/` (centralised) — rationale recorded in
  the PR description
- [ ] `docs/developers/APP_DEVELOPMENT.md` created as the Flutter contributor entry point:
  prerequisites, repo layout, link to `app/README.md`, link to emulator setup guide
- [ ] `docs/developers/DEVELOPMENT_SETUP.md` updated to mention Flutter setup and link to
  `APP_DEVELOPMENT.md`
- [ ] `docs/building.md` resolved — either merged into an appropriate existing doc and
  deleted, or moved to the correct location with updated cross-links
- [ ] All internal relative links across `docs/` and `app/docs/` verified — no broken paths
- [ ] `app/README.md` links back to `docs/developers/APP_DEVELOPMENT.md` so both entry
  points connect

## Test Plan

No automated tests required — change is non-functional (documentation only).

## Prerequisites

- **TASK-147** — `app/docs/EMULATOR_SETUP.md` must exist before the app dev entry point
  can link to it.

## Notes

The structural decision to make upfront:

| Option | Pro | Con |
|--------|-----|-----|
| Keep app docs in `app/docs/` | Close to the code; Flutter tooling paths stay simple | Developer must know to look in two trees |
| Move to `docs/developers/app/` | Single tree for all dev docs; consistent with firmware docs | `app/` dir becomes thinner; relative asset paths get longer |

Recommended default: keep guides in `app/docs/`, add `docs/developers/APP_DEVELOPMENT.md`
as a thin entry-point that links over. This avoids moving files (low churn) while solving
the discoverability problem.

## Documentation

- `docs/developers/APP_DEVELOPMENT.md` — new file (primary deliverable)
- `docs/developers/DEVELOPMENT_SETUP.md` — add Flutter section
- `docs/building.md` — merge or relocate
- `app/README.md` — add back-link to `docs/developers/APP_DEVELOPMENT.md`
