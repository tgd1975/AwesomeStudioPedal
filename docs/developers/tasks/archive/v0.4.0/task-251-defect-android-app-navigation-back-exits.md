---
id: TASK-251
title: Defect — system BACK exits the app from any sub-screen instead of popping
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (1-3h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 28
---

## Summary

On the Flutter app, the system BACK gesture / 3-button BACK exits the app
to the Android launcher from **every** sub-screen instead of popping back
to the previous screen. Discovered while running TASK-155 (AE-15) and
TASK-153 (SC-09 navigation chain). Strongly suggests routes are being
pushed with `pushReplacement` (or `go_router`'s `go`) instead of `push`.

## Reproducer

1. Launch app fresh.
2. Tap "Edit profiles" → Profile List opens.
3. Tap a profile → Profile Editor opens.
4. Tap a button (e.g. A) → Action Editor opens.
5. Press the system BACK button once.
6. Observe: app closes to the Android launcher.

Expected: BACK pops one route, returning to Profile Editor. Two more BACK
presses should land on Home and finally exit.

The same behaviour reproduces from JSON Preview, the BLE Scanner screen,
and the Action Editor.

## Tests affected

- TASK-155 AE-15: "Navigate back without tapping Save → action unchanged" —
  the data-unchanged half passes, but the navigation half fails (app exits).
- TASK-153 SC-09: "Connect, then go to Profile List, then return to Home"
  — without working BACK, "return to Home" requires either an in-app
  control or an app relaunch; the chain cannot be tested as written.
- General usability: the app feels broken because users lose their place.

## Suggested fix

Audit the navigator/router calls in `lib/` for `pushReplacement` /
`Navigator.of(...).pushReplacement` / `context.go(...)` and replace with
`push` / `context.push(...)` where backstack history should be preserved.

## Acceptance Criteria

- [x] BACK from Action Editor → Profile Editor.
- [x] BACK from Profile Editor → Profile List.
- [x] BACK from Profile List → Home.
- [x] BACK from Home → Android launcher.
- [x] Same behaviour for JSON Preview and BLE Scanner.

## Resolution

Root cause exactly as suspected: every forward navigation in the app used
`context.go(...)`, which `go_router` resolves to a *replace*-style
transition that leaves no history to pop. From any sub-screen the
system BACK gesture therefore exited the app.

Fix: replaced all forward `context.go(...)` calls with `context.push(...)`
in:

- [app/lib/screens/home_screen.dart](app/lib/screens/home_screen.dart) — 4 cards (Scan, Profiles, Community, Upload)
- [app/lib/screens/profile_list_screen.dart](app/lib/screens/profile_list_screen.dart) — JSON Preview button + profile row tap
- [app/lib/screens/profile_editor_screen.dart](app/lib/screens/profile_editor_screen.dart) — button row → Action Editor
- [app/lib/screens/upload_screen.dart](app/lib/screens/upload_screen.dart) — View JSON Preview button
- [app/lib/screens/community_profiles_screen.dart](app/lib/screens/community_profiles_screen.dart) — load-set → Profiles (`unawaited` since the future is not consumed)

The one *legitimate* replace was [app/lib/screens/scanner_screen.dart](app/lib/screens/scanner_screen.dart#L49)
where, after a successful BLE connect, the scanner used to call
`context.go('/')` to bounce back to Home. Switched to
`Navigator.of(context).pop()` so the auto-return after connect uses the
same stack the system BACK gesture would.

Regression coverage: added a new integration test
`System BACK from a sub-screen returns to Home, not exit` in
[app/test/integration/app_flow_test.dart](app/test/integration/app_flow_test.dart)
that drives the BACK pop and asserts we land back on Home with the
"Connect to pedal" card visible. All 54 → 55 tests pass; `flutter analyze`
clean.

Verification on a real device requires TASK-250 (BLE permissions) for the
SC-* test chain to actually reach the Scanner screen successfully, but the
push/pop semantics are independent of BLE and verified by the widget test.
