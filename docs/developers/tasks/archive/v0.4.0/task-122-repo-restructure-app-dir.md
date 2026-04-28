---
id: TASK-122
title: Repo Restructure — Add app/ Dir, Update CI and Devcontainer
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Support
epic: MobileApp
order: 1
---

## Description

Prepare the repository for a Flutter mobile app by creating the `app/` top-level directory, updating `.gitignore`, adding a CI workflow for the app, and extending the devcontainer with Flutter + Android SDK. This is the only task that restructures the repo — all other E tasks build inside `app/` without touching firmware directories.

## Acceptance Criteria

- [ ] `app/` directory created at repo root (Flutter project scaffold done in TASK-123)
- [ ] `.gitignore` additions: `app/build/`, `app/.dart_tool/`, `app/.flutter-plugins*`, `app/android/.gradle/`, `app/ios/Pods/`
- [ ] `.github/workflows/app.yml` created: triggers on push when files under `app/` change; steps: `flutter pub get` → `flutter analyze` → `flutter test` → `flutter build apk --release`; iOS build skipped in CI (document as manual step)
- [ ] `.devcontainer/devcontainer.json` updated: Flutter SDK (stable channel), Android SDK (command-line tools only), `flutter` and `dart` added to `PATH`, `flutter doctor` as post-create command
- [ ] `/devcontainer-sync` invoked after devcontainer changes
- [ ] `README.md` updated to reference the new `app/` directory and Flutter app
- [ ] `app/README.md` placeholder created

## Files to Touch

- `.gitignore`
- `.github/workflows/app.yml` (new)
- `.devcontainer/devcontainer.json`
- `README.md`
- `app/README.md` (new placeholder)

## Test Plan

CI workflow runs on next push. Devcontainer rebuild installs Flutter — verify with `flutter doctor` output.

## Prerequisites

None — this is the first Group E task and a prerequisite for all other E tasks.

## Notes

Android SDK: install command-line tools only (no full Android Studio) to keep the devcontainer image lean. Use `sdkmanager "platform-tools" "platforms;android-34" "build-tools;34.0.0"`. Flutter stable channel as of 2026-04-17 is 3.x — pin a specific version in the devcontainer for reproducibility.

Run `/devcontainer-sync` after editing `devcontainer.json` (project rule).
