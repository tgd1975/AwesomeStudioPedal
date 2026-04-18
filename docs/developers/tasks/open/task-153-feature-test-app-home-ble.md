---
id: TASK-153
title: Feature Test — App home screen & BLE connection
status: open
opened: 2026-04-19
effort: Small (2-4h)
complexity: Low
human-in-loop: Main
group: feature_test
---

## Description

Execute the functional and usability tests for the app home screen and BLE scanner/connection
flow as defined in `docs/developers/FEATURE_TEST_PLAN.md` Parts 2.1 and 2.2.

Tests H-01–H-03 can be run on the emulator (`Lean_API33`) without hardware.
Tests SC-01–SC-09 require a Pixel 9 (or other physical Android device) and a powered-on pedal.

## Pre-conditions

- Flutter app installed on test device (`flutter run` or `.apk`)
- ESP32 pedal flashed with current firmware, powered on and within ~5 m
- Bluetooth permission granted on Android device
- For SC-06: know how to disable Bluetooth from settings

## Tests to execute

| Test ID | Description |
|---------|-------------|
| H-01 | Fresh launch → home screen loads; 3 cards; "Not connected" in grey |
| H-02 | Upload card is greyed out; other two are active |
| H-03 | Landscape rotation → no overflow or clipped text |
| H-U1 | Usability: unfamiliar user shown home screen — what do they try first? |
| SC-01 | Tap "Connect to pedal" with pedal on → scanner shows pedal within 10 s |
| SC-02 | Signal bar reflects RSSI — changes when pedal moved closer/farther |
| SC-03 | Tap Connect → loading indicator; home shows "Connected" in green |
| SC-04 | Upload card enabled after connection |
| SC-05 | Power off pedal, wait 10 s → app reverts to "Not connected"; Upload disabled |
| SC-06 | Open scanner with Bluetooth off → clear "Bluetooth is off" message |
| SC-07 | Deny Bluetooth permission → clear permission message; no crash |
| SC-08 | Scan with pedal off → "No devices found" after timeout; no crash |
| SC-09 | Connect → navigate to Profile List → return to Home → still shows "Connected" |
| SC-U1 | Usability: time new user connecting from app launch; target < 30 s |

## Acceptance Criteria

- [ ] All H-01–H-03 and SC-01–SC-09 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] H-U1 and SC-U1 usability findings documented
- [ ] Any failures filed as child tasks using the defect template
