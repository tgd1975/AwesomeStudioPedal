---
id: TASK-082
title: User documentation review
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
group: Finalization
order: 5
---

## Description

Review all user-facing documentation for accuracy, completeness, and usability before the
first public release. Covers `README.md`, `docs/musicians/USER_GUIDE.md`,
`docs/builders/BUILD_GUIDE.md`, `docs/builders/KEY_REFERENCE.md`, and any linked pages.
The goal is to ensure that a musician with no technical background can follow the User Guide,
and a builder can follow the Build Guide, without needing to ask questions. All links,
screenshots, and cross-references must be verified against the current state of the project.

## Acceptance Criteria

- [ ] Every external and internal link in user-facing docs resolves correctly (no 404s, no
      stale anchors)
- [ ] `USER_GUIDE.md` and `BUILD_GUIDE.md` reviewed by someone who has not read the code
      and confirmed they can follow the instructions end-to-end without prior context
- [ ] All references to firmware features (profiles, LED encoding, key names, BLE pairing)
      match the current implementation; any discrepancies corrected

## Test Plan

No automated tests required — this is a documentation review task. Run `/fix-markdown` and
`/lint` before marking done to catch formatting regressions.

## Notes

- Pay attention to the simulator and config-builder links in `README.md` — these were added
  late (TASK-031, TASK-032) and should be cross-checked against the live GitHub Pages URLs.
- KEY_REFERENCE.md is especially important to review: it is the reference document for users
  building their own `profiles.json` and must match what `config_loader.cpp` actually accepts.
- If screenshots exist in `docs/media/`, verify they match the current UI/hardware.
- Run `/fix-markdown` at the end to ensure all docs pass the markdownlint rules enforced by
  the pre-commit hook.
