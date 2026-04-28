---
id: TASK-258
title: Defect — In-app BLE scan returns no devices while Android system Bluetooth picker sees the pedal
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (2-4h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: Main
epic: feature_test
order: 40
---

## Summary

After TASK-250 (manifest BLE permissions) and the related TASK-258
config change (default `data/config.json` no longer enables passkey
pairing), Android **does** see the pedal: the system Bluetooth
"neues Gerät koppeln" screen lists "AwesomeStudioPe" as a discoverable
device. But the in-app scanner (Connect to pedal → Scanning…) still
reports zero devices and the spinner times out without listing the
pedal.

This is **not** a stale-bond / cached-pairing issue — confirmed with
the user (2026-04-26): the device is unpaired and shows fresh under
"available devices for pairing" each time.

## Reproducer

1. Pedal powered on, in range, not paired with the phone.
2. Android Settings → Bluetooth → "neues Gerät koppeln":
   "AwesomeStudioPe" appears within seconds. ✓ (this confirms the pedal
   is advertising and Android sees it).
3. In the AwesomeStudioPedal app, tap **Connect to pedal**.
4. Scanner shows "Scanning for pedal…" spinner.
5. Observe: spinner runs the full 10 s timeout, returns
   "No pedal found nearby." even though the OS sees it.

## Root cause

The comment at
[lib/hardware/esp32/src/ble_config_service.cpp:128-132](lib/hardware/esp32/src/ble_config_service.cpp#L128-L132)
claims the firmware overrides the primary advertisement to "only
include our config service UUID", but no such override is actually
performed — the call is `setupGattService(pServer)`, which only
registers the GATT service, never touches the advertisement.

So the pedal's advertisement carries only what `BleKeyboard`
configures by default: the **HID UUID `0x1812`** plus the device
name. The 128-bit config service UUID (`516515c0-…`) lives in the
GATT service table — visible **after** connecting, but never
advertised. `FlutterBluePlus.startScan(withServices: [Guid(kServiceUuid)], …)`
filtered on a UUID that wasn't in any advertisement, so the
in-app scan returned zero devices on every host.

This is consistent with [docs/developers/KNOWN_ISSUES.md § BLE config
integration test disconnects on Linux with BlueZ 5.83](../../KNOWN_ISSUES.md)
which already documents the HID-UUID-in-advertisement problem from a
different angle (BlueZ auto-loading its HID profile on connect).

## Tests blocked

- TASK-153: SC-01 onwards — every test that needs the in-app scanner
  to find the pedal.
- TASK-156: UP-* tests that need a connected pedal.
- TASK-157: E2E flows that need a connected pedal.

## Acceptance Criteria

- [x] In-app scanner lists the pedal within 10 s on a Pixel 9 with
      the pedal advertising and Android Bluetooth permissions granted.
      *(structural fix verified by analyse + tests; on-device
      verification rolls up to TASK-153 SC-01 resume.)*
- [x] Root cause documented in the resolution.
- [x] If the fix changes the scan call, regression covered by either
      a unit test on the BleService scan-call shape or a comment in
      `ble_service.dart` explaining the constraint.

## Resolution

App-side fix in [app/lib/services/ble_service.dart](app/lib/services/ble_service.dart):

- Replaced `FlutterBluePlus.startScan(withServices: [Guid(kServiceUuid)], …)`
  with an unfiltered `startScan` followed by client-side filtering on
  device-name prefix `kPedalNamePrefix` (case-insensitive,
  `startsWith` to tolerate vendor-specific name truncation in scan
  responses). Results are also de-duplicated by `remoteId` so the same
  pedal seen on multiple advertisement packets only appears once.
- `kServiceUuid` is **still used** by `BleService.connect()` to find
  the config GATT service after the connection is established — that
  service does live in the GATT table once connected, just not in the
  advertisement.
- Added [app/lib/constants/ble_constants.dart](app/lib/constants/ble_constants.dart)
  `kPedalNamePrefix = 'AwesomeStudio'` (a 13-char prefix), with a doc
  comment pointing at this task and KNOWN_ISSUES.md for the why.

### Follow-up after on-device verification (2026-04-26 18:00–18:15)

Driving the new build on a Pixel 9 / Android 16 via adb-uiautomator
exposed two additional problems that the static-source fix above
did not catch. Both were resolved here in the same task:

**Issue 1 — `androidLegacy: true` is required.**
`flutter_blue_plus` 1.36.8 defaults `androidLegacy: false`, which uses
Android 12+'s extended-advertising scan API. The pedal's ESP32 NimBLE
stack only does **legacy 1M-PHY advertising**, so the extended-mode
scan never surfaces it. Logcat confirmed this: `startScan` was issued
and ran the full 10 s timeout without a single `onScanResult`
callback, while a parallel scan from another tool found the pedal
fine. Added `androidLegacy: true` to the `startScan` call.

**Issue 2 — Local Name truncation makes the 18-char prefix wrong.**
Android reports the pedal's `platformName` as `AwesomeStudioPe`
(16 chars). Comparing against the original 18-char
`kPedalNamePrefix = 'AwesomeStudioPedal'` with
`platformName.startsWith(prefix)` always returned false because the
prefix is longer than the device name. Shortened the constant to
`'AwesomeStudio'` (13 chars), which survives any reasonable
truncation while still being uniquely ours.

After both fixes, end-to-end on the Pixel 9: scan finds
`AwesomeStudioPe` within 10 s, tapping Connect succeeds, the home
screen updates to "Connected", and GATT subscription to the config
status characteristic (`516515c3-…`) returns `GATT_SUCCESS` —
verified in logcat.

Added a third static-source guard test in
[app/test/unit/ble_service_scan_filter_test.dart](app/test/unit/ble_service_scan_filter_test.dart)
that fails if `androidLegacy: true` is removed from the `startScan`
call, since the failure mode (silent zero-results scan) would
otherwise be invisible in CI.

The cleaner fix would be in the firmware — actually do the
advertisement override the comment promises, adding the config
service UUID to the advertisement so a `withServices:` filter works.
That is deliberately **not** done here:

1. Removing the HID UUID from the advertisement would break the BLE
   keyboard role on hosts that auto-bond on HID discovery
   (TASK-229's territory).
2. Adding both HID and the 128-bit config UUID to the same 31-byte
   primary advertisement risks overflow into the scan response, which
   re-creates a similar visibility problem on hosts that don't merge
   primary + scan-response.
3. The Linux BlueZ disconnect bug documented in KNOWN_ISSUES.md is
   tied to the HID UUID being advertised, not to the config UUID
   missing — fixing the advertisement does not unblock that path.

The app-side filter-by-name change unblocks TASK-153/154/156 on
Android and iOS today; revisiting the firmware advertising layout
is a separate, lower-priority cleanup.

### Coverage

- [app/test/unit/ble_service_scan_filter_test.dart](app/test/unit/ble_service_scan_filter_test.dart) —
  static-source guard with two assertions:
  - `BleService.scan` source must NOT contain `withServices:` outside
    of comments (regression guard against re-introducing the filter).
  - The source MUST reference `kPedalNamePrefix` (positive guard that
    the new filter is in place).

`flutter test` 74 → 76 tests, all green; `flutter analyze` clean.

### Why a static-source test instead of a behavioural one

`flutter_blue_plus` cannot be exercised without the radio, and the
existing tests already mock `BleService` wholesale (so a behavioural
test would mock away the very call we want to assert about). A
static-source assertion is the smallest thing that catches the most
likely regression — someone "fixing" the missing UUID filter — and
runs in CI for free.

## Notes

- This was the second-layer issue after TASK-250 (manifest) and the
  TASK-250 config follow-up (Just Works pairing). With TASK-258
  resolved, the in-app scan path should be unblocked end-to-end.
- Cross-reference: BlueZ HID auto-load + ATT 0x0E races in
  [docs/developers/KNOWN_ISSUES.md](../../KNOWN_ISSUES.md), passkey
  enforcement history in TASK-237/246.
