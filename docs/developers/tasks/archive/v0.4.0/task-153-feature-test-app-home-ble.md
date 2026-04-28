---
id: TASK-153
title: Feature Test — App home screen & BLE connection
status: closed
closed: 2026-04-27
opened: 2026-04-19
effort: Small (2-4h)
effort_actual: Small (<2h)
complexity: Low
human-in-loop: Main
epic: feature_test
order: 5
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

- [x] All H-01–H-03 and SC-01–SC-09 functional tests executed and results recorded in FEATURE_TEST_PLAN.md
- [ ] H-U1 and SC-U1 usability findings documented — **deferred to a non-developer tester sitting; not a blocker for functional acceptance**
- [x] Any failures filed as child tasks using the defect template (TASK-262, TASK-263 — both closed)

## History

- 2026-04-26: Resumed after TASK-251, TASK-253, TASK-258 closed.
- 2026-04-26 (round 2 paused): Most tests passed; SC-06 and SC-07 failed
  → TASK-262 filed; SC-01 device-name truncation → TASK-263 filed.
- 2026-04-27 (close): Re-verified SC-06 and SC-07 on Pixel 9 with
  TASK-262 fix in place — both now show clear empty-state UI with
  CTA. Smoke-checked SC-01/SC-03/SC-04/SC-09 to confirm no regression
  from the TASK-263 reclassification (BLE 4.x Local Name cap is
  platform-imposed; the Tooltip + ellipsis treatment is in source).
  H-U1/SC-U1 usability checks deferred to a non-developer sitting.
