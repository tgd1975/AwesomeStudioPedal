---
id: TASK-354
title: Firmware — expose firmware-version read characteristic (+ DIS 0x180A decision)
status: active
opened: 2026-05-01
effort: Small (<2h)
complexity: Medium
human-in-loop: Clarification
epic: app-content-pages
order: 12
prerequisites: [TASK-353]
decision_doc: docs/developers/BLE_READBACK_IMPACT.md
---

## Description

Expose the firmware version over BLE so the Connected-Pedal page
(TASK-336) can fill its currently-placeholdered Firmware row.

**Platform scope: ESP32 only.** nRF52840 hardware is not currently
available for verification, and shipping the (cheap) Bluefruit
`BLEDis` path without on-device confirmation is rejected. The
nRF52840 firmware-version surface is deferred to
[TASK-358](task-358-nrf52840-ble-readback-surfaces.md), which
bundles all three nRF52840 readback surfaces under one custom-
Bluefruit-GATT-service infra spike.

What ships (per [BLE_READBACK_IMPACT.md §3.1](../../BLE_READBACK_IMPACT.md#31-firmware-version-read-characteristic--task-354)):

- **ESP32**: new READ characteristic in `BleConfigService` at UUID
  `516515c5-4b50-447b-8ca3-cbfce3f4d9f8`, value `FIRMWARE_VERSION`
  from [include/version.h](../../../include/version.h).
- **DIS on ESP32**: skipped — would duplicate the firmware-version
  string in a parallel service for no Connected-Pedal-page benefit.

App-side:

- Wire `BleService.readDeviceFirmwareVersion()` (or extend
  `readDeviceHardware` pattern) and surface the result on the
  Connected-Pedal page's Firmware row, replacing the
  `_PendingRow` widget.

## Acceptance Criteria

- [x] Firmware-version characteristic exists on **ESP32** at UUID
      `516515c5-…` and returns the canonical version string from
      `FIRMWARE_VERSION`. nRF52840 deferred to TASK-358.
- [x] UUID `…5c5` registered in
      [BLE_CONFIG_PROTOCOL.md](../../BLE_CONFIG_PROTOCOL.md#characteristics).
      *(Added rows for both `HW_IDENTITY` (`…5c4`) and `FIRMWARE_VERSION`
      (`…5c5`) — `…5c4` was previously undocumented.)*
- [x] App's Connected-Pedal page Firmware row renders the live
      string for ESP32-connected pedals, "—" for nRF52840-connected
      pedals (deferred), "—" when disconnected. *(Widget test
      `connected_pedal_screen_test.dart` covers all three paths.)*
- [x] On-device test verifies the characteristic returns the
      expected string format on ESP32 (extends
      [test/test_ble_config_esp32/](../../../test/test_ble_config_esp32/)).
      *(Added "Test 5" in `runner.py` reading
      `CHAR_FIRMWARE_VERSION_UUID` and matching `^v\d+\.\d+\.\d+`.
      Pending hardware verification by user via `/test-device`.)*

## Prerequisites

- **TASK-353** — feasibility analysis must land first; this task
  picks up the DIS decision and the per-platform implementation
  sketch.

## Test Plan

**On-device tests** — extend the existing
`test/test_ble_config_esp32/` BLE test folder with a read of the
new characteristic. Pattern follows the existing
`kHwIdentityUuid` test in that folder.

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
- Hardware: ESP32 USB-connected for on-device runs (nRF52840 deferred to TASK-358).
