---
id: TASK-273
title: Defect — Firmware does not reboot after CONFIG_WRITE_HW upload
status: closed
closed: 2026-04-27
opened: 2026-04-26
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Medium
human-in-loop: Verify
epic: feature_test
order: 41
---

## Description

Surfaced while running TASK-156 UP-08 round 3 on 2026-04-26 (Pixel 9, Android 16).

The Feature Test Plan UP-08 expects: "Tap **Upload Hardware Config**
→ Progress and success/failure shown; **pedal reboots** after upload."
The reboot is necessary because hardware-pin assignments (button
pins, LED pins, button counts) are read at firmware boot — changing
them at runtime would leave the pedal half-initialised against the
old mapping.

In the post-TASK-267 round-3 run, the in-app flow worked end-to-end:
import a fresh `config.json`, tap **Upload Hardware Config**, see
"Hardware config uploaded!" SnackBar within 1 s. **However:** the
BLE link stayed live — a follow-up Upload Profiles still succeeded
on the same connection — and the firmware's CONFIG_WRITE_HW handler
in
[lib/hardware/esp32/src/ble_config_service.cpp](../../../../lib/hardware/esp32/src/ble_config_service.cpp)
contains no `esp_restart()` (or NVS-then-reset sequence) after the
write. So the new mapping is persisted to flash but is not applied
until the next manual power cycle.

Net effect: a user uploading a corrected pin map sees the success
SnackBar, assumes the pedal is now using the new map, and is silently
running on the old (cached, boot-loaded) mapping — which can include
buttons wired to the wrong GPIOs, LEDs that no longer light up, or
counts that mismatch the JSON. The same is presumably true for the
nRF52840 path — both firmware targets should reboot symmetrically.

## Reproducer

1. Pixel 9 / Android 16, app post-TASK-267 build.
2. Connect to pedal, import any valid `config.json` via the new
   Profile List → More actions → Import Hardware Config menu.
3. Open Upload screen → tap **Upload Hardware Config**.
4. Observe: "Hardware config uploaded!" SnackBar in ~1 s.
5. Tap **Upload Profiles** again — succeeds on the same BLE link
   (proves the link is still up; the pedal did not reboot).
6. Inspect
   [lib/hardware/esp32/src/ble_config_service.cpp](../../../../lib/hardware/esp32/src/ble_config_service.cpp)
   — no `esp_restart()` in the CONFIG_WRITE_HW handler.

## Acceptance Criteria

- [x] After a successful CONFIG_WRITE_HW write+commit, the ESP32
      firmware schedules a reboot via `esp_restart()` (or equivalent
      controlled-shutdown sequence) so the new pin mapping is loaded
      on the next boot.
- [x] The nRF52840 firmware does the same, symmetrically.
      **N/A** — `BleConfigService` is `#ifdef ESP32` only
      ([src/main.cpp](../../../../src/main.cpp#L38)); the nRF52840
      build has no upload path, so there is nothing to reboot from.
      When the nRF path is added, it must inject a reboot callback
      into `BleConfigReassembler` the same way ESP32 does.
- [x] The reboot is delayed long enough for the firmware to send the
      `OK` status notification to the host (so the app's UploadResult
      success path runs and the SnackBar shows) and for NVS/storage
      to flush. ~50–250 ms after status notify is enough.
      Implemented as: `notifyStatus("OK")` → `blinkSuccess()` (3 ×
      150 ms on/off = 900 ms) → `esp_restart()`. The blink doubles as
      the BLE-notify flush window.
- [x] The app's existing post-upload disconnect handling (the
      `BluetoothConnectionState.disconnected` listener in
      [app/lib/services/ble_service.dart](../../../../app/lib/services/ble_service.dart#L70))
      gracefully re-renders Home as "Not connected" when the reboot
      drops the link — no error dialog, just the natural disconnect.
      Verified on-device 2026-04-27 via `/verify-on-device UP-08` on
      Pixel 9 / Android 16: Home re-rendered with the Bluetooth tile
      "Not connected" and the Upload card disabled (grayed out), no
      dialog.
- [x] FEATURE_TEST_PLAN.md UP-08 column updated to ✓ on the next
      TASK-156 round (or a follow-up round) once the reboot is in
      place. Round 4 added 2026-04-27.

## Test Plan

**On-device tests** (`make test-esp32-config` and
`make test-nrf52840-config` if a config-test target exists; otherwise
covered by manual verification + CI build smoke):

- Extend the relevant test or add a new on-device check that fires
  CONFIG_WRITE_HW and verifies the device boots cleanly afterwards.
  May be covered indirectly: most on-device tests boot the device,
  so the post-reboot side is implicit.
- Requires: ESP32 (and ideally nRF52840) connected via USB.

**Manual on-device verification via `/verify-on-device` UP-08:**

- Re-run the round-3 sequence.
- Expected: SnackBar "Hardware config uploaded!" → pedal LED
  status briefly drops → BLE link disconnects within ~1–2 s of the
  SnackBar → Home re-renders as "Not connected" → next scan finds
  the pedal advertising again with the new mapping in effect.

## Notes

- Cross-reference: TASK-156 round-3 results in
  [docs/developers/tasks/active/task-156-feature-test-app-upload-preview.md](../active/task-156-feature-test-app-upload-preview.md)
  and the round-3 UP-08 column in
  [docs/developers/FEATURE_TEST_PLAN.md](../../FEATURE_TEST_PLAN.md).
- Cross-reference: TASK-267 (which made UP-08 reachable in the first
  place) — closed 2026-04-26.
- Open question for triage: should the test plan instead drop the
  reboot expectation and document that a manual power-cycle is
  required? That is also a valid design choice (less surprise from
  unexpected disconnects) but requires the app to surface a
  "Power-cycle the pedal to apply" hint after the SnackBar. The
  acceptance criteria above assume the reboot path; flip them if
  the design choice goes the other way.
- Surfaced in TASK-156 UP-08 round 3 on 2026-04-26.
- Verified on-device on 2026-04-27 via `/verify-on-device UP-08` on
  Pixel 9 / Android 16. Screenshots: `/tmp/up08_now.png` (Upload
  screen mid-disconnect, both upload buttons grayed out),
  `/tmp/up08_home.png` (Home re-rendered to "Not connected" with no
  error dialog after the reboot).
