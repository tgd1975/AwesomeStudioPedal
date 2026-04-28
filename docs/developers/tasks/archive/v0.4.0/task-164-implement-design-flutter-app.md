---
id: TASK-164
title: Implement design — Flutter mobile app
status: closed
closed: 2026-04-20
opened: 2026-04-19
effort: Large (8-24h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: None
epic: design
order: 2
---

## Description

Apply the completed design system to the Flutter mobile app (`app/`).
The Flutter theme scaffold is in `docs/design/handoff/flutter/asp_theme.dart`.
The component spec is in `docs/design/handoff/components/COMPONENTS.md`.

**Decision: dark mode only.** `ThemeMode.dark` is enforced; no light theme.
This matches the web tools and the stage/studio use context from the design brief.

## Scope

### 1 — Theme setup

- Copy `docs/design/handoff/flutter/asp_theme.dart` → `app/lib/theme/asp_theme.dart`
- In `app/lib/app.dart`: replace the current `ThemeData(colorScheme: ColorScheme.fromSeed(...))`
  with `AspTheme.dark`, set `themeMode: ThemeMode.dark`, remove the light `theme:` entry

### 2 — Font sourcing (licensing note)

Both fonts are SIL OFL licensed — freely usable in this open-source project.

| Font | License source | Strategy |
|------|---------------|----------|
| **Metropolis** | OFL — [github.com/chrismsimpson/Metropolis](https://github.com/chrismsimpson/Metropolis) | Bundle OTF under `assets/fonts/metropolis/`; declare in `pubspec.yaml` |
| **JetBrains Mono** | OFL — Google Fonts / JetBrains | Bundle OTF files under `assets/fonts/jetbrains-mono/`; declare in `pubspec.yaml` |

Download from the respective GitHub release pages. Weights needed — Metropolis: Regular (400),
Medium (500), SemiBold (600), Bold (700). JetBrains Mono: Regular (400), Medium (500).

Add to `pubspec.yaml` under `flutter: fonts:` as shown in the comment block at the top of
`asp_theme.dart`.

### 3 — Component checklist

Apply token-based styling to each of the following. Replace hardcoded `Colors.xxx` with
`Theme.of(context).colorScheme.*` or `AspTokens.*` constants.

#### App-wide

- [ ] `app/lib/app.dart` — theme wired, `ThemeMode.dark` set
- [ ] All `AppBar` instances — surface color, amber icon/title tint
- [ ] All `ElevatedButton` — amber fill (`AspTokens.accent`), dark ink
- [ ] All `TextButton` — amber text color
- [ ] All `InputDecoration` — dark fill (`AspTokens.surface`), amber focus ring, `radius-sm`
- [ ] All `DropdownButtonFormField` / `DropdownButton` — dark fill, amber selection

#### Home screen (`home_screen.dart`)

- [ ] Action cards (`_ActionCard`) — surface color, `radius-card`, correct border
- [ ] BLE status chip — pill badge, color-coded: grey (disconnected), pulsing amber
  (scanning), green (connected); build as a reusable `BleSStatusChip` widget

#### Profile list screen (`profile_list_screen.dart`)

- [ ] List tiles — spacing, divider color, monospace index badge (amber)
- [ ] Validation banner — full green/red sticky banner (already exists as widget)
- [ ] Upload FAB — amber

#### Profile editor screen (`profile_editor_screen.dart`)

- [ ] Expandable button sections — consistent tile height, surface background

#### Action editor screen (`action_editor_screen.dart`)

- [ ] Dropdown — full-width, dark fill, amber chevron
- [ ] Parameter fields — consistent spacing, amber focus ring
- [ ] Save / Cancel hierarchy (primary vs ghost button)

#### Scanner screen (`scanner_screen.dart`)

- [ ] Device list tiles — RSSI bar color
- [ ] Scanning indicator — amber

#### Upload screen (`upload_screen.dart`)

- [ ] Progress bar — amber (`AspTokens.accent`)
- [ ] Success / error state — token colors

#### Community profiles screen (`community_profiles_screen.dart`)

- [ ] Cards — surface color, amber tag chips
- [ ] Search field — dark fill, amber focus

#### JSON preview screen (`json_preview_screen.dart`)

- [ ] Background `AspTokens.bg`, text `AspTokens.text`, monospace font

## Acceptance Criteria

- [ ] `app/lib/theme/asp_theme.dart` exists; `App` widget uses `AspTheme.dark`
- [ ] Font files present under `assets/fonts/`; `pubspec.yaml` declares them
- [ ] `flutter analyze` passes with no errors or warnings
- [ ] All existing Flutter tests pass (`make flutter-test`)
- [ ] No hardcoded `Color(0xFF...)` or `Colors.xxx` values remain outside `asp_theme.dart`
- [ ] App is dark-mode only; verified in Android emulator (API 33) and/or iOS simulator

## Notes

- Visual-only refactor — do not change business logic or navigation
- Widget tests that assert on specific colors may need minor updates; update them, do not
  weaken them
- The `BleStatusChip` widget built here should be added to `app/test/widget/` with a
  minimal rendering test
