---
id: TASK-166
title: Scaffold profiles/ folder structure and CONTRIBUTING.md
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: No
epic: CommunityProfiles
order: 1
---

## Description

Create the `profiles/` directory with four subfolders (`1-button/`, `2-button/`,
`3-button/`, `4-button/`) and a stub `profiles/index.json`. Write
`profiles/CONTRIBUTING.md` based on the "Call for Contributions" section of IDEA-017.

No profile set files yet — those come in TASK-167.

## Acceptance Criteria

- [ ] `profiles/1-button/`, `profiles/2-button/`, `profiles/3-button/`, `profiles/4-button/` exist and are tracked in git (add a `.gitkeep` if needed)
- [ ] `profiles/index.json` stub created: `{ "version": 1, "generated": "", "profiles": [] }`
- [ ] `profiles/CONTRIBUTING.md` written with contribution guidelines from IDEA-017 (what makes a good profile, step-by-step how to contribute, terminology section reference)

## Notes

See IDEA-017 "Repository Structure" and "Call for Contributions" for the exact content.
