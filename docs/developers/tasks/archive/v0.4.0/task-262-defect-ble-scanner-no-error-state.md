---
id: TASK-262
title: Defect — BLE scanner shows indefinite spinner when Bluetooth is off or permission is denied
status: closed
closed: 2026-04-27
opened: 2026-04-26
effort: Small (2-4h)
effort_actual: Medium (2-8h)
complexity: Low
human-in-loop: Main
epic: feature_test
order: 33
---

## Description

Discovered while running TASK-153 SC-06 and SC-07 on 2026-04-26 (after
TASK-258 unblocked the happy-path BLE scan).

The BLE scanner screen has only two terminal states:

1. Pedal found → list with `Connect` button.
2. No pedal found → "No pedal found nearby." with `Scan again`
   button. (SC-08 — works.)

There is no terminal state for the two failure modes the user is most
likely to hit on a real Android phone:

- **Bluetooth is off on the phone** (SC-06). The scanner shows
  `Scanning for pedal…` spinner indefinitely (>20 s observed). No
  "Bluetooth is off" message, no enable prompt, no timeout.
- **Bluetooth permission denied** at the system prompt (SC-07). After
  tapping "Don't allow" / "Nicht zulassen", the scanner shows the same
  `Scanning for pedal…` spinner indefinitely. No "Permission needed"
  message, no link to app settings, no timeout. App does not crash.

Both cases share a UX root cause: `flutter_blue_plus` (or however the
scan is wired up) returns no events, and the screen has no error
listener — only a "no results yet" spinner. The user is stuck.

## Reproducer

**SC-06 (BT off):**

1. `adb shell pm clear com.example.awesome_studio_pedal`
2. `adb shell svc bluetooth disable`
3. Launch app, tap "Connect to pedal".
4. Observe: indefinite `Scanning for pedal…` spinner, no error.

**SC-07 (permission denied):**

1. `adb shell pm clear com.example.awesome_studio_pedal`
2. Launch app, tap "Connect to pedal".
3. On the system permission dialog, tap "Don't allow".
4. Observe: indefinite `Scanning for pedal…` spinner, no error.

## Acceptance Criteria

- [ ] When Bluetooth is off at scan time, the scanner shows a clear
      "Bluetooth is off" message with an "Enable Bluetooth" button (or
      a deep-link to system Bluetooth settings if direct enable is not
      possible on Android 13+).
- [ ] When Bluetooth permission is denied, the scanner shows a clear
      "Permission needed to scan for pedal" message with a button that
      either re-requests the permission or deep-links to the app's
      Permissions screen in system Settings (handle the
      "permanently denied" case gracefully).
- [ ] Both states update reactively if the user toggles the
      condition (e.g. enables BT from notification shade) — no need
      to back out and re-enter the scanner.
- [ ] No crash, no ANR.

## Test Plan

Manual on-device re-run of TASK-153 SC-06 and SC-07 with a Pixel 9 (or
any Android 12+ device):

- SC-06: BT-off path produces "Bluetooth is off" UI, not the spinner.
- SC-07: permission-denied path produces "Permission needed" UI with
  a working settings deep-link, not the spinner.
- Toggling BT back on from the notification shade or granting the
  permission via settings should let the scanner recover without a
  full app restart.

No host tests required — this is presentation/state-management code in
the Flutter app. Widget tests for the scanner state machine would be
nice-to-have but are out of scope unless the existing app test setup
already supports them.

## Notes

- Surfaced in TASK-153 (Feature Test — Home + BLE) re-run on
  2026-04-26. SC-06 and SC-07 were the only two failures in an
  otherwise green re-run.
- Related to (but not the same as) TASK-258, which fixed the *happy
  path* — pedal found while everything is in order. This task covers
  the *unhappy paths* the user will trip over once they start using
  the app on different phones.
- Consider whether `flutter_blue_plus.adapterState` and the
  `Permission.bluetoothScan` / `Permission.bluetoothConnect` checks
  should live in a single `BleAvailability` controller that the
  scanner subscribes to — the same controller would also unblock a
  future Home-screen badge "Bluetooth is off — tap to enable" idea.

## Resolution

Fixed in commit on `feature/idea-realizations` (2026-04-27):

- [app/lib/services/ble_service.dart](../../../../app/lib/services/ble_service.dart):
  added pre-flight permission check (`hasBlePermissions`,
  `requestBlePermissions`, `isBlePermissionPermanentlyDenied`) and
  adapter-state guard before `FlutterBluePlus.startScan`. Also added a
  `scanResults.listen(onError: …)` so async scan failures (mid-scan
  permission revocation, etc.) surface as a settable `lastError`
  rather than silent empty results.
- [app/lib/screens/scanner_screen.dart](../../../../app/lib/screens/scanner_screen.dart):
  subscribes to `BleService.adapterState` and auto-re-scans when the
  user toggles Bluetooth back on; the "Open App Settings" button
  re-requests permissions first (handles transient denial) and falls
  through to `openAppSettings()` only when the OS marks them
  permanently denied.
- New dep: `permission_handler` (Android 12+ `BLUETOOTH_SCAN`/
  `BLUETOOTH_CONNECT` runtime checks; FBP itself does not expose
  these).
- Static-source guard tests added under
  [app/test/unit/ble_service_scan_filter_test.dart](../../../../app/test/unit/ble_service_scan_filter_test.dart)
  for the adapterState check, the async scanResults listener, and
  the permission pre-flight.

Verified on-device on 2026-04-27 on Pixel 9 / Android 16:

- **SC-06 (BT off):** "Bluetooth is off" card visible within 1 s of
  tapping "Connect to pedal"; toggling BT on auto-resumed the scan
  with no manual back-and-re-enter. Screenshots:
  `/tmp/verify_TASK-155_sc06_bt_off.png`,
  `/tmp/verify_TASK-155_sc06_recovery.png`.
- **SC-07 (permission denied):** system prompt appears on first scan
  attempt; tapping "Don't allow" surfaces the "Bluetooth permission
  denied" card with "Open App Settings"; tapping re-requests on
  transient denial, then falls back to `openAppSettings()` if
  permanently denied; granting both BLUETOOTH_SCAN and
  BLUETOOTH_CONNECT auto-resumed the scan. Screenshots:
  `/tmp/verify_TASK-155_sc07_perm_denied.png`,
  `/tmp/verify_TASK-155_sc07_recovery_scanning.png`.

The TASK-153 SC-06 and SC-07 columns in
[docs/developers/FEATURE_TEST_PLAN.md](../../FEATURE_TEST_PLAN.md)
will flip from ✗ to ✓ on the next TASK-153 round.
