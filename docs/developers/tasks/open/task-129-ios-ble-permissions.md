---
id: TASK-129
title: iOS BLE Permissions and Build Verification
status: open
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: Support
group: MobileApp
order: 12
prerequisites: [TASK-122]
---

## Description

Add required iOS BLE permission keys to `Info.plist` and set the minimum deployment target so `flutter_blue_plus` compiles cleanly for iOS. Verify the iOS build locally (no CI — requires macOS + Xcode).

## Acceptance Criteria

- [ ] `app/ios/Runner/Info.plist` contains `NSBluetoothAlwaysUsageDescription` and `NSBluetoothPeripheralUsageDescription` with meaningful strings
- [ ] `app/ios/Podfile` sets `platform :ios, '12.0'` minimum deployment target
- [ ] `flutter build ios --no-codesign` succeeds locally (manual step — document in `app/README.md`)
- [ ] `app/README.md` documents the iOS manual build process and the dual-BLE-service behaviour (HID handled by system, config GATT accessible to app)

## Files to Touch

- `app/ios/Runner/Info.plist`
- `app/ios/Podfile`
- `app/README.md`

## Test Plan

Manual on macOS with Xcode: `flutter build ios --no-codesign` must complete without error. Mark as `[manual-test-required]`.

## Prerequisites

- **TASK-122** — `app/` directory and Flutter scaffold must exist

## Notes

On iOS, CoreBluetooth will see both the HID profile and the custom GATT config service. The HID profile is handled by the system's Bluetooth HID driver transparently; the custom GATT service is accessible to the app via `CBCentralManager` without conflict. Document this in `app/README.md` to avoid confusion during testing.
