---
id: TASK-159
title: Refactor Makefile — add app targets and improve help presentation
status: open
opened: 2026-04-19
effort: Small (2-4h)
complexity: Low
human-in-loop: None
group: dx
---

## Description

The Makefile has grown organically and has two gaps:

1. **Missing Flutter app targets** — there is no way to run or test the mobile app in its
   three modes (Chromium web, Android emulator, physical device) from `make`.
2. **Help is hard to scan** — the `all` target prints a single long column; with 30+ commands
   it is easy to miss relevant targets.

This task covers both gaps in one refactor pass.

## New Flutter targets to add

| Target | Command it should run |
|--------|-----------------------|
| `flutter-run-chrome` | `cd app && flutter run -d chrome` |
| `flutter-run-emulator` | `cd app && flutter run -d <emulator-id>` (document how to get ID with `flutter devices`) |
| `flutter-run-device` | `cd app && flutter run` (picks the first connected physical device) |
| `flutter-build-ios` | `cd app && flutter build ios --release` |
| `flutter-build-web` | `cd app && flutter build web --release` |

All new targets should guard against `flutter` not being on PATH (same pattern as existing targets).

## Help layout options

Consider one of the following approaches and pick whichever reads best:

**Option A — Two-column layout** (printf-based, no dependencies):
```
  make build              Build ALL targets          make flutter-run-chrome   Run app in Chromium
  make clean              Clean artifacts            make flutter-run-emulator Run app on emulator
  ...
```

**Option B — Grouped with a divider line** (current approach, but with the new Flutter
section added and consistent column width using `printf '  %-30s %s\n'`):
```
Flutter App Commands:
  flutter-get              Install Flutter dependencies
  flutter-run-chrome       Run app in Chromium (web, no BLE)
  flutter-run-emulator     Run app on Android emulator
  flutter-run-device       Run app on connected physical device
  flutter-test             Run Flutter unit/widget tests
  flutter-analyze          Run flutter analyze
  flutter-build            Build release APK
  flutter-build-ios        Build release iOS app (macOS only)
  flutter-build-web        Build release web app
```

**Option C — `help` target using `##` comment extraction** (self-documenting pattern):
Each target gets a `## Description` comment; `make help` greps and formats them.
This keeps target definitions and documentation co-located and never out of sync.
This is the most maintainable approach for a large Makefile.

## Acceptance Criteria

- [ ] `make flutter-run-chrome` starts the app in Chromium (or prints a clear error if flutter not found)
- [ ] `make flutter-run-emulator` starts the app on a running Android emulator
- [ ] `make flutter-run-device` runs on a connected physical device
- [ ] `make flutter-build-ios` triggers an iOS release build
- [ ] `make flutter-build-web` triggers a web release build
- [ ] `make` (default / `all`) displays the full command list in a readable two-column or grouped format
- [ ] All existing targets still work unchanged
- [ ] `make help` or `make` shows new targets

## Notes

- Do not remove the dev-container warning that explains Flutter is not installed inside the
  container — it saves confusion for people running `make` inside VS Code's devcontainer terminal.
- The emulator target may need `EMULATOR_ID` as an overridable variable:
  `make flutter-run-emulator EMULATOR_ID=emulator-5554`
