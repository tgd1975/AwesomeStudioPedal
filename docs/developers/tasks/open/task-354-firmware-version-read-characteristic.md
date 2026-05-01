---
id: TASK-354
title: Firmware — expose firmware-version read characteristic (+ DIS 0x180A decision)
status: open
opened: 2026-05-01
effort: Small (<2h)
complexity: Medium
human-in-loop: Clarification
epic: app-content-pages
order: 12
prerequisites: [TASK-353]
---

## Description

Expose the firmware version over BLE so the Connected-Pedal page
(TASK-336) can fill its currently-placeholdered Firmware row.

What ships:

- **Firmware-version read characteristic** — short string from
  `include/version.h` (e.g. `"0.4.2 abcdef0"`). New UUID alongside
  `kHwIdentityUuid`. Read-only, no security implications.
- **DIS (0x180A)** — bundle or skip per the TASK-353 decision.

App-side:

- Wire `BleService.readDeviceFirmwareVersion()` (or extend
  `readDeviceHardware` pattern) and surface the result on the
  Connected-Pedal page's Firmware row, replacing the
  `_PendingRow` widget.

## Acceptance Criteria

- [ ] Firmware-version characteristic exists on both ESP32 and
      nRF52840 and returns the canonical version string.
- [ ] UUID registered in the BLE protocol doc per the TASK-353
      output.
- [ ] DIS (0x180A) decision implemented per TASK-353 (bundle or
      skip).
- [ ] App's Connected-Pedal page Firmware row is no longer a
      placeholder — renders the live string when connected, "—"
      when disconnected.
- [ ] On-device test verifies the characteristic returns the
      expected string format on at least one of the two targets.

## Prerequisites

- **TASK-353** — feasibility analysis must land first; this task
  picks up the DIS decision and the per-platform implementation
  sketch.

## Test Plan

**On-device tests** — extend the existing
`test/test_*_esp32/` and/or `test/test_*_nrf52840/` BLE test
folder with a read of the new characteristic. Pattern follows
the existing `kHwIdentityUuid` test (whichever target has one
today; if neither does, add one for the platform you're flashing
during this task and document the gap for the other).

**Host tests** — the version-string format itself is host-testable
behind `HOST_TEST_BUILD`. Add a small unit test if the formatting
logic is non-trivial (e.g. concatenating semver + git hash).

**App-side widget test** — extend the existing
`test/widget/connected_pedal_screen_test.dart` to assert the
Firmware row reflects a mocked `readDeviceFirmwareVersion()`
return value.

## Notes

- This was carved out of the original TASK-337 — the smallest of
  the three deliverables. Cheap, read-only, finishes one of the
  three placeholder rows on the Connected-Pedal page.
- Hardware: ESP32 + nRF52840 USB-connected for on-device runs.
