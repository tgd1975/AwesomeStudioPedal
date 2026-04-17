---
id: TASK-123
title: Flutter Project Scaffold and Navigation
status: open
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: MobileApp
order: 2
prerequisites: [TASK-122, TASK-115, TASK-116]
---

## Description

Initialise the Flutter project inside `app/`, configure dependencies, set up navigation with `go_router`, and embed the JSON schema assets so offline validation works from the start.

## Acceptance Criteria

- [ ] `app/pubspec.yaml` created with all required dependencies (see Notes)
- [ ] `app/lib/main.dart` and `app/lib/app.dart` created; app launches without error
- [ ] Dark/light `MaterialApp` theme matches web config-builder colour variables: accent `#2563eb`, background `#f5f5f5`
- [ ] `go_router` configured with named routes: `/`, `/scan`, `/profiles`, `/profile/:id`, `/action/:buttonId`, `/upload`, `/json-preview`
- [ ] `data/profiles.schema.json` and `data/config.schema.json` embedded as Flutter assets in `pubspec.yaml`
- [ ] `app/analysis_options.yaml` created with strict linting rules
- [ ] `flutter analyze` passes with zero errors
- [ ] `flutter test` passes (even with no test cases yet)

## Files to Touch

- `app/pubspec.yaml` (new)
- `app/lib/main.dart` (new)
- `app/lib/app.dart` (new)
- `app/analysis_options.yaml` (new)

## Test Plan

`flutter analyze` and `flutter test` in `app/` must pass.

## Prerequisites

- **TASK-122** — `app/` directory and devcontainer must exist
- **TASK-115** — `profiles.schema.json` must exist to embed as asset
- **TASK-116** — `config.schema.json` must exist to embed as asset

## Notes

Dependencies (`pubspec.yaml`):
```yaml
dependencies:
  flutter_blue_plus: ^1.x
  file_picker: ^6.x
  share_plus: ^7.x
  provider: ^6.x
  go_router: ^13.x
  json_schema: ^5.x
  path_provider: ^2.x

dev_dependencies:
  flutter_test:
  mockito: ^5.x
  build_runner: ^2.x
```

Assets section:
```yaml
assets:
  - ../data/profiles.schema.json
  - ../data/config.schema.json
```
