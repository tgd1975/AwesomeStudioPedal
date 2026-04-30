---
id: TASK-345
title: Swap Android launcher icon to ASP mark
status: open
opened: 2026-04-30
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
epic: LogoBranding
order: 7
---

## Description

The Flutter Android app currently ships with the generic Flutter launcher icon
across all `mipmap-*` density buckets. Replace it with the ASP mark so the app
icon on the device home screen, app drawer, and recents matches the rest of the
project surfaces.

This is heavier than the other LogoBranding tasks because it requires
generating five PNG densities (mdpi/hdpi/xhdpi/xxhdpi/xxxhdpi) plus an adaptive
icon (foreground + background layers) for Android 8+, and verifying the result
on a real device.

## Acceptance Criteria

- [ ] `asp-mark.svg` rasterised to all required Android densities, replacing `app/android/app/src/main/res/mipmap-*/ic_launcher.png`
- [ ] Adaptive icon (foreground + background) added under `mipmap-anydpi-v26/` so Android 8+ renders correctly with system-shaped masks
- [ ] App rebuilt and installed on the Pixel; launcher, app drawer, and recents all show the ASP mark
- [ ] iOS icon untouched (out of scope — separate task if/when iOS ships)

## Test Plan

No automated tests required — change is non-functional (asset swap).

Manual verification on the connected Pixel via `/verify-on-device`:
- Reinstall the app, confirm the launcher icon on the home screen and app drawer is the ASP mark
- Open the app and confirm Recents shows the ASP mark
- Long-press the icon and confirm the adaptive-icon variants (squircle, circle, square) all render without clipping the mark

## Notes

- Recommended tool: `flutter_launcher_icons` package — drives the rasterisation from a single source and updates `pubspec.yaml`. Verify it's available before scripting; if not, ask.
- The mark is currently a single-colour amber on white; the adaptive-icon background layer needs a deliberate choice (white? amber? transparent?) — flag to the user during implementation.
- iOS asset catalogue is **not** in scope — a follow-up task can add iOS once that target is active.
