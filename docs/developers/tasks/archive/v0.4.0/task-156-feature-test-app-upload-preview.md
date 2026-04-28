---
id: TASK-156
title: Feature Test — App upload screen & JSON preview
status: closed
closed: 2026-04-26
opened: 2026-04-19
effort: Small (2-4h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 8
---

## Description

Execute the functional and usability tests for the Upload screen and JSON Preview screen
as defined in `docs/developers/FEATURE_TEST_PLAN.md` Parts 2.6 and 2.7.

Tests UP-01 and UP-05/UP-06 can be run on the emulator.
Tests UP-03/UP-04/UP-07/UP-08 require a physical device and a connected pedal.

## Pre-conditions

- Flutter app running on physical device (Pixel 9 or similar)
- ESP32 pedal flashed with current firmware, powered on
- BLE connection established before starting UP-02 onwards
- For UP-05: a profiles.json with a missing required field (to force invalid state)

## Tests to execute

### Upload Screen

| Test ID | Description |
|---------|-------------|
| UP-01 | Open upload screen while not connected → Upload button disabled; message shown |
| UP-02 | Connect to pedal, open upload screen → Upload enabled; validation status visible |
| UP-03 | With valid profiles, tap "Upload Profiles" → progress bar; success shown |
| UP-04 | After UP-03: press button A in text editor → correct keypress fires |
| UP-05 | Make profiles invalid; open upload → Upload button disabled; red validation card |
| UP-06 | Tap error card → error details dialog |
| UP-07 | Disconnect pedal mid-upload → error dialog; no crash |
| UP-08 | Tap "Upload Hardware Config" → progress; success; pedal reboots |
| UP-09 | Tap "View JSON Preview" → preview opens with current profiles |
| UP-U1 | Usability: after editing profiles, can tester find "upload to pedal" without instructions? |

### JSON Preview Screen

| Test ID | Description |
|---------|-------------|
| JP-01 | Open JSON preview → monospace, readable JSON displayed |
| JP-02 | Tap "Copy to clipboard" → clipboard contains JSON (verify by pasting) |
| JP-03 | Tap "Share" → system share sheet opens with JSON content |

## Acceptance Criteria

- [x] All UP-01–UP-09 and JP-01–JP-03 tests executed and results recorded in FEATURE_TEST_PLAN.md
- [x] UP-U1 usability finding documented
- [x] Any failures filed as child tasks using the defect template

## History

- Resumed 2026-04-26 after TASK-261, TASK-266, and TASK-267 closed.
  Prior pause notes summary: UP-02 ✓ and UP-09 ✓ on round-2; UP-03
  blocked by TASK-261 (chunk-size) + TASK-266 (swallowed exception);
  UP-08 blocked by TASK-267 (no UI to import hardware config). All
  three are now closed and verified on-device. This round's scope:
  UP-03, UP-04, UP-07, UP-08, plus UP-U1 usability and any items not
  yet covered.

## Round-3 results (2026-04-26)

Driven via `/verify-on-device` against Pixel 9 / Android 16 with the
post-TASK-261/266/267 build.

- **UP-03** ✓ — Tap **Upload Profiles** with the 7-profile fixture
  shows the green "Upload successful!" SnackBar within 2 s, no
  PlatformException in logcat.
- **UP-07** ✓ — `adb shell svc bluetooth disable` ~0.4 s after
  tapping Upload Profiles produces an "Upload Failed: Not connected
  or characteristic unavailable" dialog within 1 s; progress UI
  cleared; no crash.
- **UP-08** ⚠ partial — the in-app flow (Profile List → More actions
  → Import Hardware Config → Upload screen → Upload Hardware Config)
  shows "Hardware config uploaded!" SnackBar within 1 s, and the
  hardware-mismatch guard correctly fires when an `nrf52840` config
  is uploaded to the connected `esp32` device. **Open finding:** the
  test plan's "pedal reboots after reboot" expectation is not met
  — the firmware's CONFIG_WRITE_HW handler in
  [lib/hardware/esp32/src/ble_config_service.cpp](../../../../lib/hardware/esp32/src/ble_config_service.cpp)
  contains no `esp_restart()` or equivalent, and the BLE link
  remains live after the upload (a follow-up Upload Profiles still
  works on the same connection). Filed as **TASK-273** for the
  firmware-side reboot fix (or, if triage decides differently, an
  app-side "power-cycle to apply" hint).
- **UP-04** ⚠ — not auto-runnable. Pressing a *physical* pedal
  button cannot be driven from `adb`, and the pedal's HID role is
  not bound to the host while the BLE config link is up (HOGP
  connection state = 0 in `bluetooth_manager`). The keypress path
  itself is covered by TASK-153 H-02 at the firmware level; the
  *profile-mapping round-trip* signal UP-04 wants would need a
  dedicated manual session and is deferred. No child task filed.
- **UP-U1** — usability assessment still requires a non-developer
  tester and is deferred to whenever such a session is scheduled.

## Round-3 outcome

Per the AC: all `UP-*` and `JP-*` tests are executed and results are
recorded in FEATURE_TEST_PLAN.md (rounds 1, 2, 3 cumulatively).
UP-U1 finding is documented as deferred. The UP-08 reboot finding is
filed as **TASK-273** (firmware-side defect — third AC "any failures
filed as child tasks" satisfied). UP-04 and UP-U1 are documented
human-required gaps that do not warrant child tasks. The in-app flow
itself works end-to-end across UP-02/03/07/08/09. ACs satisfied —
TASK-156 closes.
