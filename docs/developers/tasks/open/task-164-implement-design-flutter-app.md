---
id: TASK-164
title: Implement design — Flutter mobile app
status: open
opened: 2026-04-19
effort: Large (8-24h)
complexity: Medium
human-in-loop: Main
group: design
---

## Description

Apply the visual design defined in TASK-162 to the Flutter mobile app (`app/`).
This covers theming, component restyling, and layout polish across all screens.

**Depends on:** TASK-162 (design tokens and wireframes must be completed first)

## Scope

### Theme setup

- [ ] Create (or extend) `app/lib/theme.dart` with a `ThemeData` that encodes the TASK-162
  design tokens as Flutter `ColorScheme`, `TextTheme`, `CardTheme`, `InputDecorationTheme`,
  `ElevatedButtonThemeData`, etc.
- [ ] Replace any hardcoded `Colors.xxx` values in widget files with theme lookups
  (`Theme.of(context).colorScheme.xxx`)
- [ ] Decide on light vs dark mode (per TASK-162 decision); if dark: add `darkTheme` to `MaterialApp`

### Per-screen changes

Follow the wireframes from TASK-162 for each screen:

#### Home screen (`app/lib/screens/home_screen.dart`)

- [ ] Card styling matches design spec (radius, shadow, colour)
- [ ] BLE status chip consistent colour with success/error tokens
- [ ] Typography hierarchy (title, subtitle, caption) applied

#### Profile list screen (`app/lib/screens/profile_list_screen.dart`)

- [ ] List tile styling (spacing, divider, drag handle)
- [ ] Validation banner uses theme error/success colours
- [ ] Import/Export buttons styled as secondary actions

#### Profile editor screen (`app/lib/screens/profile_editor_screen.dart`)

- [ ] Button rows: consistent tile height, leading icon, trailing chevron

#### Action editor screen (`app/lib/screens/action_editor_screen.dart`)

- [ ] Dropdown styled to match `InputDecorationTheme`
- [ ] Type-specific value fields: consistent spacing and label style
- [ ] Save / Cancel button placement and hierarchy (primary vs text button)

#### Scanner screen (`app/lib/screens/scanner_screen.dart`)

- [ ] Device list tiles: signal bar colour consistent with strength
- [ ] "Scanning…" loading indicator style

#### Upload screen (`app/lib/screens/upload_screen.dart`)

- [ ] Progress bar colour from theme
- [ ] Status card (valid / invalid) uses theme success/error colours

#### JSON preview screen

- [ ] Monospace font from theme (`--font-mono` equivalent)
- [ ] Background and text use dark code preview colours

### App icon

- [ ] Replace the default Flutter blue icon with a project-specific icon
  (foot-pedal silhouette or similar; source from TASK-162 design assets)
- [ ] Generate all required sizes using `flutter_launcher_icons` package or manual export:
  - Android: `mipmap-mdpi` through `mipmap-xxxhdpi`
  - iOS: `AppIcon.appiconset` (all sizes including 1024×1024)

## Acceptance Criteria

- [ ] `theme.dart` created; all screens use theme tokens — no hardcoded `Colors.xxx` remaining
- [ ] App icon updated on both Android and iOS
- [ ] All screens match TASK-162 wireframes
- [ ] `flutter analyze` passes with no errors
- [ ] All existing Flutter tests still pass (`make flutter-test`)
- [ ] Manually tested on Pixel 9 and iPhone (or Lean_API33 emulator + iOS simulator for layout)

## Notes

- This is a visual-only refactor — do not change business logic or navigation structure
- Widget tests may need minor updates if they assert on specific colours or widget types
  that change as part of the restyling (update the tests, do not weaken them)
- Keep accessibility in mind: ensure sufficient contrast ratios (WCAG AA minimum)
