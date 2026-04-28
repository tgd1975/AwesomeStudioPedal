---
id: TASK-165
title: Flutter/Dart code quality pipeline — pre-commit hooks, CI, skills, and Makefile targets
status: closed
closed: 2026-04-20
opened: 2026-04-19
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Support
---

## Description

The embedded firmware side of this project has a mature code-quality pipeline: host unit
tests run on every commit via a pre-commit hook, clang-format enforces style, and CI
validates everything on push. The Flutter/Dart app (`app/`) has basic test coverage but
is missing the same guardrails: `dart format` is not enforced at commit time, `flutter
analyze` does not run pre-commit, and there are no convenience skills for running app
checks interactively.

This task adds the missing layer so that every commit touching `app/` is held to the
same minimum-quality bar as the firmware.

## Acceptance Criteria

- [ ] Pre-commit hook runs `dart format --output=none --set-exit-if-changed` and
  `flutter analyze` for any commit that includes staged files under `app/`; both
  checks are skipped gracefully when `flutter` is not on PATH (dev-container scenario).
- [ ] `make flutter-format` Makefile target auto-formats all Dart files in `app/`.
- [ ] CI workflow runs `dart format --output=none --set-exit-if-changed` and
  `flutter analyze` on every pull request that touches `app/`.

## Test Plan

No automated tests required — change is non-functional (tooling/CI/scripts only).

Manual verification:

1. Stage a Dart file with a formatting violation → pre-commit hook should reject the commit.
2. Stage a correctly-formatted Dart file → pre-commit hook should pass.
3. Run `make flutter-format` → Dart files in `app/` are reformatted in-place.
4. Push a branch with an `app/` change → CI flutter-quality job passes (or fails on a
   deliberate formatting error).

## Notes

- The pre-commit hook (`scripts/pre-commit`) already runs `flutter test` for staged
  `app/` files (lines 170-185). Add the format and analyze checks in the same
  guarded block so the PATH-missing skip applies to all three.
- `dart format --output=none --set-exit-if-changed .` exits non-zero when any file
  would be reformatted — suitable for a hook check without modifying files.
- `flutter analyze` exits non-zero on warnings at default severity. If the existing
  `app/analysis_options.yaml` is too strict or too lax, adjust it as part of this task.
- The CI job should run only on `paths: ['app/**']` to avoid triggering on firmware-only
  changes.
- Skills `/test-app` and `/lint-app` are optional stretch goals; the core acceptance
  criteria above are sufficient for a closed task.
