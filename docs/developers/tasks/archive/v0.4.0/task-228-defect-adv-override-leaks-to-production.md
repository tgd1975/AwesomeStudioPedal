---
id: TASK-228
title: Defect — BLE advertisement override leaks from test build into production, breaks HID discoverability
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: Support
epic: feature_test
order: 16
---

## Description

`BleConfigService::setupGattService` ([lib/hardware/esp32/src/ble_config_service.cpp](../../../../lib/hardware/esp32/src/ble_config_service.cpp))
**unconditionally** replaced the BLE advertisement with a custom payload that
contains **only** the Config service UUID, stripping HID (`0x1812`) and the
keyboard appearance value from the advertisement in production firmware:

```cpp
NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
NimBLEAdvertisementData advData;
advData.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
advData.setCompleteServices(NimBLEUUID(SERVICE_UUID));
adv->setAdvertisementData(advData);
adv->setScanResponse(false);
```

The override was originally introduced as a workaround for
[BLE_CONFIG_IMPLEMENTATION_NOTES.md Challenge 3](../../BLE_CONFIG_IMPLEMENTATION_NOTES.md):
BlueZ's HID daemon auto-connects to devices advertising `0x1812` and breaks our
on-device Python integration test runner. That concern is **test-only** — a
Linux integration-test concern, not a product requirement.

Leaving the override in production had these real-world effects:

- Android's system Bluetooth picker filters BLE peripherals by advertised
  appearance/UUIDs and did not surface the pedal as a keyboard.
- Pairing attempts from `bluetoothctl` completed the BLE connect step but then
  failed with `org.bluez.Error.AuthenticationFailed` during the Security
  Manager handshake, in part because the OS could not recognise the pedal
  as an HID device up-front.
- The primary product UX — "plug in pedal, pair to phone, use as BT
  keyboard, no app required" — did not work on Android or Linux.

A prior, sibling defect was caught in commit `8485f69` ("fix(ble): restore
production security; scope no-auth to test build only"). That commit wrapped
`NimBLEDevice::setSecurityAuth(false, false, false)` in
`#ifdef BLE_CONFIG_TEST_BUILD` but **missed the advertisement override** right
next to it. Same class of bug, same guard needed.

## Fix applied

Wrapped the advertisement override in `#ifdef BLE_CONFIG_TEST_BUILD` alongside
the existing `setSecurityAuth` guard. Production firmware now keeps the
`BleKeyboard`-library default advertisement (HID UUID + keyboard appearance +
name). Test firmware (`BLE_CONFIG_TEST_BUILD`) retains the slimmed-down
Config-only advertisement so the Python integration test runner is unchanged.

## Acceptance Criteria

- [x] Advertisement override moved inside `#ifdef BLE_CONFIG_TEST_BUILD`.
- [x] Production firmware rebuilt and flashed; `bluetoothctl info` shows
      `Appearance: 0x03c1`, `Icon: input-keyboard`, and
      `UUID: Human Interface Device (00001812-…)`.
- [x] Android system Bluetooth picker discovers and pairs the pedal without a
      PIN prompt (Just Works via `BleKeyboard` defaults).
- [x] On-device BLE integration test (`test_ble_config_esp32`) still passes
      (7/7) — the `#ifdef` guard keeps the override active in the test env.

## Test Plan

**On-device tests** (`make test-esp32-ble-config` or equivalent):

- Re-run the existing BLE config integration test suite. It runs with
  `BLE_CONFIG_TEST_BUILD` defined and therefore still gets the stripped
  advertisement → BlueZ HID daemon still does not auto-grab → tests unchanged.

No host tests required — the fix is in platform-specific firmware code that
cannot be shimmed.

## Notes

- **Root-cause style**: this is the second instance of a test-only behaviour
  leaking into production from the same function. Consider moving all
  test-only overrides (security + advertisement) out of
  `ble_config_service.cpp` entirely and into the test-firmware's entry point,
  so the pattern cannot recur.
- Android pairing was confirmed to succeed without a PIN prompt, which
  matches the `BleKeyboard` default posture (bonding + MITM + SC with
  `NoInputNoOutput` capability → Just Works accepted on platforms whose
  SMP policy allows it).
- Linux `bluetoothctl` pairing was **not** re-tested yet in this task.
  Whether it now succeeds with the default `BleKeyboard` security is a
  separate question — TASK-151 will surface it.
