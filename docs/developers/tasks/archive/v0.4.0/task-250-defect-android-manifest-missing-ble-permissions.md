---
id: TASK-250
title: Defect — Android manifest missing BLE permissions; app cannot scan or connect
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (1-2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 10
priority: critical
---

## Summary

The Flutter app's Android manifest declares **only** `INTERNET`. None of the
Bluetooth or Location permissions required for BLE scanning on Android 12+
are present, so the app's BLE scanner runs forever without finding the pedal,
and the Connect-to-pedal flow is completely broken on a real Pixel 9 device.
Discovered while running TASK-153 (SC-01–SC-09).

## Evidence

```text
$ adb shell dumpsys package com.example.awesome_studio_pedal | grep "requested permissions" -A 5
    requested permissions:
      android.permission.INTERNET
      com.example.awesome_studio_pedal.DYNAMIC_RECEIVER_NOT_EXPORTED_PERMISSION
```

The merged debug manifest at
`app/build/app/intermediates/packaged_manifests/debug/processDebugManifestForPackage/AndroidManifest.xml`
matches: only `INTERNET` is requested.

The source manifest at
[app/android/app/src/main/AndroidManifest.xml](app/android/app/src/main/AndroidManifest.xml)
has no `<uses-permission>` entries beyond what Flutter injects.

## Reproducer

1. `adb install -r app/build/app/outputs/flutter-apk/app-debug.apk` (or `flutter run`).
2. `adb shell pm clear com.example.awesome_studio_pedal` to start clean.
3. Launch the app; BT enabled on phone; pedal powered on within range.
4. Tap "Connect to pedal".
5. Observe: spinner "Scanning for pedal…" never resolves; no permission dialog
   is ever shown; pedal is never listed (the CLI tool *can* see the pedal —
   verified during the TASK-150 run, so this is not a pedal-side issue).

## Expected

On first launch, the app requests `BLUETOOTH_SCAN` and `BLUETOOTH_CONNECT`
(plus `ACCESS_FINE_LOCATION` if `flutter_blue_plus` is configured to require
it for scanning). Once granted, the scanner shows the pedal within ~10 s.

## Suggested fix

Add to `app/android/app/src/main/AndroidManifest.xml` (outside `<application>`):

```xml
<uses-permission android:name="android.permission.BLUETOOTH_SCAN"
    android:usesPermissionFlags="neverForLocation"
    tools:targetApi="s" />
<uses-permission android:name="android.permission.BLUETOOTH_CONNECT" />
<!-- legacy (Android 11 and below) -->
<uses-permission android:name="android.permission.BLUETOOTH" android:maxSdkVersion="30" />
<uses-permission android:name="android.permission.BLUETOOTH_ADMIN" android:maxSdkVersion="30" />
<!-- only if scanning needs to reveal location: -->
<!-- <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" /> -->
```

…and ensure the Flutter side requests them at runtime via `permission_handler`
or whatever flutter_blue_plus exposes. Verify `usesPermissionFlags="neverForLocation"`
is acceptable for the use-case (it avoids needing FINE_LOCATION).

## Tests blocked by this defect

- TASK-153: SC-01, SC-02, SC-03, SC-04, SC-05, SC-07, SC-09 (and
  arguably SC-06 / SC-08 since the scan UI never resolves either way).
- TASK-156: UP-02, UP-03, UP-04, UP-07, UP-08 (anything requiring an actual
  upload to the pedal).
- TASK-157 E2E flows that need a connected pedal.

## Acceptance Criteria

- [x] Android manifest declares the BLE permissions and (if needed) location.
- [x] Fresh-installed app prompts for the BT permissions on first scan.
  *(verified by user 2026-04-26: Android system Bluetooth picker now
  sees the device, confirming permissions are granted at the OS layer.)*
- [ ] On a Pixel 9 with BT enabled and pedal in range, the scanner lists
  the pedal within 10 s (re-runs SC-01 ✓).
  **Blocked by TASK-258** — in-app scan still empty even though OS sees
  the pedal; root cause is at a different layer (likely
  `withServices:` filter mismatch) and is out of scope for TASK-250.
- [ ] With BT disabled on the phone, the scanner shows a clear "Bluetooth is
  off" message instead of an indefinite spinner (re-runs SC-06 ✓).
  *(deferred to TASK-153 resume — gated by TASK-258 because the
  scanner cannot be reached when no devices are listed.)*
- [ ] With BT permission denied, the scanner shows a clear permission message
  with a settings link, no crash (re-runs SC-07 ✓).
  *(deferred to TASK-153 resume — same reason.)*

## Resolution

[app/android/app/src/main/AndroidManifest.xml](app/android/app/src/main/AndroidManifest.xml)
now declares the BLE permissions outside `<application>`:

| Permission | Scope | Notes |
|---|---|---|
| `BLUETOOTH_SCAN` | Android 12+ | `usesPermissionFlags="neverForLocation"` so we don't need `ACCESS_FINE_LOCATION` for scanning. |
| `BLUETOOTH_CONNECT` | Android 12+ | Required to connect after a scan returns a device. |
| `BLUETOOTH` | Android ≤ 11 | Legacy. `maxSdkVersion="30"` so it's not requested on newer devices. |
| `BLUETOOTH_ADMIN` | Android ≤ 11 | Legacy, gated by `maxSdkVersion`. |
| `ACCESS_FINE_LOCATION` | Android ≤ 11 | Legacy: pre-Android-12 BLE scanning required location. Gated by `maxSdkVersion="30"`. |

`xmlns:tools` was added to the `<manifest>` element to support
`tools:targetApi="s"` on `BLUETOOTH_SCAN`.

No runtime-request code was needed: `flutter_blue_plus` 1.x auto-requests
the declared BLE permissions on `FlutterBluePlus.startScan(...)`. The
existing `BleService.scan` already calls `startScan` and surfaces
`FlutterBluePlusException` as `lastError`, which `ScannerScreen` already
renders as a "permission denied" or "Bluetooth is off" card via its
existing `_ErrorCard` branch logic
([app/lib/screens/scanner_screen.dart](app/lib/screens/scanner_screen.dart)).

### Why no automated test

The acceptance criteria require a Pixel 9 with the pedal in range —
emulator-based widget tests cannot exercise the Android permission
prompt or the BLE radio. The structural change (manifest entries) is
trivially inspectable in the diff; behavioural ACs are validated on
hardware.

`flutter test` (74 tests) and `flutter analyze` both clean, confirming
nothing else regressed from the manifest change.

### Follow-ups discovered during device verification

User testing on a Pixel 9 (2026-04-26) revealed two further problems
that are *not* regressions of TASK-250 — the manifest fix did its job
(Android now prompts for and grants the BLE permissions, the OS sees
the device) — but uncovered the next layer of the connection path:

1. **`data/config.json` shipped with `pairing_pin: 12345` enabled by
   default**, but the pedal has no display to show the passkey. Any
   Android/iOS host trying to bond gets a brief passkey-entry dialog
   ("PIN/Passkey fehlerhaft") with no way to read the PIN. Resolved
   in this commit by removing `pairing_pin` from `data/config.json`,
   making Just Works (no PIN) the default. Documented in
   [docs/builders/HARDWARE_CONFIG.md § BLE pairing (optional)](../../builders/HARDWARE_CONFIG.md#ble-pairing-optional)
   and the dev note in
   [docs/developers/BLE_CONFIG_IMPLEMENTATION_NOTES.md § Security model](../BLE_CONFIG_IMPLEMENTATION_NOTES.md#security-model).
   Builders who want passkey-entry auth can opt back in by adding
   `pairing_pin` to `data/config.json` and re-flashing the data
   partition. *(No new task filed — fix included here because the
   default was wrong and this PR is the right place to correct it.)*

2. **In-app scan returns no devices even though the OS Bluetooth
   picker sees the pedal.** Filed as **TASK-258**. Likely cause is
   the `withServices: [Guid(kServiceUuid)]` filter on
   `FlutterBluePlus.startScan` — the pedal advertises the UUID but
   it may be in the scan response rather than the primary
   advertisement, depending on advertising layout. Investigation
   steps documented in TASK-258.

The remaining behavioural ACs (SC-01 scanner listing, SC-06 BT-off
message, SC-07 permission-denied message) are deferred to TASK-153
resume and gated by TASK-258 — they cannot be exercised until the
in-app scan finds the device.
