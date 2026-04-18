---
id: TASK-158
title: Feature Test — Build, deploy and test the iOS app on iPhone
status: open
opened: 2026-04-19
effort: Medium (4-8h)
complexity: Medium
human-in-loop: Main
group: feature_test
---

## Description

Build the Flutter app for iOS, deploy it to a physical iPhone, and run the full feature
test suite against it. The iOS target uses `flutter_blue_plus` for BLE and the same
Dart codebase as Android, but requires macOS + Xcode and an Apple Developer account.

This task covers both the build/deploy setup and the feature verification.

## Pre-conditions

### Build environment (macOS required)

- macOS 13 (Ventura) or later
- Xcode 15+ installed via the App Store (`xcode-select --install` for CLT)
- `flutter doctor` passes with no iOS errors
- CocoaPods installed: `sudo gem install cocoapods`
- Apple Developer account (free account is enough for device testing via Xcode direct install;
  paid account required for TestFlight distribution)
- iPhone running iOS 12.0 or later, connected via USB and trusted
- `flutter doctor --android` can be skipped; `flutter doctor -v` must show ✓ for iOS toolchain

### App / hardware

- ESP32 pedal flashed with current firmware
- iPhone with Bluetooth enabled and permission granted to the app

## Setup steps

1. On macOS, clone the repo and run `flutter pub get` in `app/`
2. `cd app/ios && pod install` — installs native CocoaPods dependencies
3. Open `app/ios/Runner.xcworkspace` in Xcode (not `.xcodeproj`)
4. In Xcode → Runner target → Signing & Capabilities: set your Apple Team
5. Connect iPhone, select it as the run target
6. `flutter run` (or run from Xcode) — app installs on the iPhone

## Permissions required in Info.plist

Verify these keys are present in `app/ios/Runner/Info.plist` (add if missing):

| Key | Purpose |
|-----|---------|
| `NSBluetoothAlwaysUsageDescription` | BLE access |
| `NSBluetoothPeripheralUsageDescription` | Legacy BLE (iOS < 13) |

## Tests to execute

Run the same functional tests as the Android pass, specifically those that differ or
are higher-risk on iOS:

### Build & launch

| Test ID | Step | Expected result | Pass? |
|---------|------|----------------|-------|
| iOS-01 | `flutter build ios --release` (or Xcode Archive) | Build completes with no errors | |
| iOS-02 | Install on iPhone via `flutter run` or Xcode | App launches; home screen loads | |
| iOS-03 | First launch: system Bluetooth permission dialog appears | Grant permission; app continues normally | |
| iOS-04 | Deny Bluetooth permission, then re-open | Clear message; link or guidance to Settings | |

### BLE connection (mirrors SC-01–SC-09 on Android)

| Test ID | Step | Expected result | Pass? |
|---------|------|----------------|-------|
| iOS-SC-01 | Tap "Connect to pedal" with pedal on | Scanner opens; pedal found within 10 s | |
| iOS-SC-02 | Tap Connect | Loading indicator; home shows "Connected" in green | |
| iOS-SC-03 | Power off pedal, wait 10 s | App detects disconnect; Upload card disabled | |
| iOS-SC-04 | Scan with Bluetooth disabled | "Bluetooth is off" message; no crash | |

### Profile management (mirrors PL and PE)

| Test ID | Step | Expected result | Pass? |
|---------|------|----------------|-------|
| iOS-PL-01 | Add, edit, reorder, delete profiles | All operations work identically to Android | |
| iOS-PL-02 | Import a `profiles.json` via Files app share sheet | Profiles load; green validation banner | |
| iOS-PL-03 | Export profiles → share sheet → save to Files | File appears in Files app; is valid JSON | |

### Action editor

| Test ID | Step | Expected result | Pass? |
|---------|------|----------------|-------|
| iOS-AE-01 | Tap through all action types | Dropdowns, text fields, autocomplete all work | |
| iOS-AE-02 | Save an action and upload to pedal | Pedal fires correct keypress | |

### Upload

| Test ID | Step | Expected result | Pass? |
|---------|------|----------------|-------|
| iOS-UP-01 | Upload profiles via BLE | Progress bar; success; pedal reflects profiles | |
| iOS-UP-02 | Press button A after upload in a text editor on a Mac | Correct keypress fires over BLE | |
| iOS-UP-03 | Disconnect mid-upload | Error dialog; no crash | |

### iOS-specific checks

| Test ID | Step | Expected result | Pass? |
|---------|------|----------------|-------|
| iOS-X-01 | Background the app while connected, return | BLE connection still present (or graceful reconnect) | |
| iOS-X-02 | Rotate to landscape | No overflow or clipped text | |
| iOS-X-03 | Test on iPad (if available) | UI scales acceptably; no crash | |

## E2E Musician Workflow

Run at least E2E-01 (Score Navigator) and E2E-02 (Media Playback) from TASK-157 on the iPhone.
These verify the core musician use case on iOS end-to-end.

## Acceptance Criteria

- [ ] `flutter build ios` succeeds on macOS
- [ ] App installs and launches on physical iPhone
- [ ] All iOS-01–iOS-04 (build/launch) tests pass
- [ ] All iOS-SC-01–iOS-SC-04 (BLE) tests pass
- [ ] iOS-PL-01–iOS-PL-03 (profiles) pass
- [ ] iOS-AE-01–iOS-AE-02 (action editor) pass
- [ ] iOS-UP-01–iOS-UP-03 (upload) pass
- [ ] iOS-X-01–iOS-X-03 (iOS-specific) checked
- [ ] E2E-01 and E2E-02 completed on iPhone
- [ ] Any failures filed as child tasks using the defect template in FEATURE_TEST_PLAN.md

## Notes

- The iOS simulator does **not** support BLE — a physical iPhone is required for any
  BLE-dependent test (SC, UP, AE-02, E2E)
- File import/export on iOS uses the system Files share sheet rather than the Android
  file picker — verify the `file_picker` plugin version supports iOS correctly
- If `pod install` fails due to a CocoaPods version mismatch, try `pod repo update` first
