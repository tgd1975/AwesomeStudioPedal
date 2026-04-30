---
id: TASK-342
title: Add ASP logo to mobile app AppBar
status: open
opened: 2026-04-30
effort: Small (<2h)
complexity: Junior
human-in-loop: No
epic: logo-branding
order: 4
---

## Description

The Flutter mobile app's `HomeScreen` AppBar currently shows only the
"AwesomeStudioPedal" text title. Add the ASP mark to the left of the title so
the brand is visible whenever the app is open.

Per IDEA-049 decisions: add the logo to the AppBar; do not replace the title
text. The launcher icon swap is handled separately in TASK-345.

## Acceptance Criteria

- [ ] `asp-mark.svg` (or a PNG raster of it) added to `app/assets/` and registered in `pubspec.yaml`
- [ ] `HomeScreen` AppBar shows the mark to the left of the title text, sized to the AppBar's standard icon area
- [ ] App still builds and runs on the connected Pixel via the existing Flutter run flow

## Test Plan

No automated tests required — change is non-functional (visual asset).

Manual verification via `/verify-on-device` or `/ui-dump`: launch the app on the
Pixel, confirm the mark appears next to the title on the home screen, and that
no layout regressions occur on smaller screen widths.

## Notes

- Flutter SVG support requires `flutter_svg`; if not already a dependency, prefer rasterising the mark to PNG at 1×/2×/3× and shipping those, since the mark is small.
- App entry: [app/lib/screens/home_screen.dart](app/lib/screens/home_screen.dart).
- Asset source: `docs/design/handoff/logo/asp-mark.svg`.
