---
id: TASK-260
title: Unify version numbers across all deliverables (firmware, app, CLI, simulator, …)
status: open
opened: 2026-04-26
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Today every deliverable has its own version number, and they have already drifted:

| Deliverable | Current version | Source of truth |
|---|---|---|
| Firmware | `v0.3.0` | `include/version.h` (`FIRMWARE_VERSION`) |
| Flutter app | `1.0.0+1` | `app/pubspec.yaml` |
| Root `package.json` | `1.0.0` | `package.json` |
| Task system tooling | (separate) | `awesome-task-system/VERSION` |
| CLI / simulator | (none yet — but coming) | TBD |

The decision is to **align all of them on one project-wide version number** — even when a particular deliverable hasn't changed for several bumps in a row. The Flutter app gets `v0.4.0` next release whether or not any Dart code was touched. The simplification is worth more than the technical-correctness of "this artifact really is unchanged since v0.2.0."

This task wires that policy through the codebase, the `/release` skill, and the documentation.

## Acceptance Criteria

- [ ] One canonical version source is chosen and documented (e.g. `VERSION` at repo root, or keep `include/version.h` as the source). All other version fields read from or are bumped in lockstep with it.
- [ ] `/release` skill updates **every** deliverable's version field in one step: `include/version.h`, `app/pubspec.yaml`, root `package.json`, `awesome-task-system/VERSION`, and any CLI/simulator manifests that exist at release time.
- [ ] All currently-drifted versions are aligned to the next release (`v0.4.0` or whatever the next bump is) — this task does **not** retro-bump for past releases.
- [ ] `docs/developers/CI_PIPELINE.md` (or wherever release process is documented) explicitly states the policy: *"All deliverables share the same version number. We do not skip-bump unchanged artifacts."*
- [ ] The `README.md` (and/or a relevant doc the user lands on) makes the same statement so external readers understand why the Flutter app jumps from 1.0.0 to v0.4.0.

## Test Plan

No automated tests required — change is non-functional (release tooling + documentation).

Manual verification at next release cut:

- Run `/release vX.Y.Z` against a clean tree.
- After the dry run, confirm every file in the canonical list shows `vX.Y.Z` (or the equivalent format-shifted form: e.g. pubspec uses `X.Y.Z+B`, gradle uses `versionName`).
- Confirm CHANGELOG entry mentions the unified-versioning policy on the first release that applies it.

## Notes

- **Format mismatches per ecosystem.** Flutter wants `1.0.0+1`, npm wants `1.0.0`, firmware uses the `vX.Y.Z` literal. The release script needs a small mapping table — pick one canonical `MAJOR.MINOR.PATCH` and project it into each format.
- **The Flutter `+B` build counter.** `pubspec.yaml` versions look like `1.0.0+1` where `+1` is the Android `versionCode`. Decision needed: does the build counter reset on every version bump, or increment forever? Recommend **increment forever** — Google Play requires a strictly increasing `versionCode`, and resetting it complicates Play Store uploads.
- **First-bump jump is large.** Flutter app goes 1.0.0 → 0.4.0 (a *downgrade* in semver terms). Two options:
  - Hard reset to 0.4.0 and live with the explanatory note in the changelog.
  - Skip ahead — pick a unified version higher than every existing one, e.g. `v1.1.0`, so no deliverable goes backwards.
  Prefer the second option for the app specifically; semver-decreasing a published artifact causes problems with package managers and app stores. **Open question for the user.**
- **Out of scope.** No automated check that all versions agree (a CI guard could be a follow-up). No version-bump tooling beyond extending the existing `/release` skill. No semantic-release / conventional-commit automation.
- **Related.** TASK-179 (determine Android app release approach) and TASK-160 (publish to Play Store) both intersect with versioning policy and should be revisited after this task lands.

## Documentation

- `docs/developers/CI_PIPELINE.md` — add a "Version policy" section: one number, all deliverables, no skip-bumps.
- `README.md` — short paragraph in the project structure / releases section noting that all deliverables ship under one version.
- `.claude/skills/release/SKILL.md` — update the bump step to cover every version-bearing manifest, with the mapping table (canonical → per-ecosystem format).
