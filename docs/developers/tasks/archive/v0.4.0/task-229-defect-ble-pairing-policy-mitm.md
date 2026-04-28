---
id: TASK-229
title: Defect — BLE pairing policy (MITM=true) prevents Linux/Windows pairing of a no-display pedal
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Support
epic: feature_test
order: 17
---

## Description

The `BleKeyboard` library default security posture is `setSecurityAuth(true, true, true)` —
bonding + **MITM** + Secure Connections. Our pedal has no display and no keypad, so its
BLE I/O Capability is `NoInputNoOutput`.

When a central pairs with a peripheral that requires MITM but has `NoInputNoOutput`, the
Security Manager cannot choose a pairing method that provides MITM protection:

- Just Works — no MITM → rejected by the peripheral because MITM is required.
- Passkey Entry — requires a keypad or display → we have neither.
- Numeric Comparison — requires a display → we have none.
- Out-of-band (OOB) — requires NFC/USB side-channel → we do not implement this.

The result is a stalemate that different OS stacks resolve differently:

- **Android** accepts Just Works for devices that advertise as HID and proceeds without
  MITM regardless of the peripheral's flag. Pairing succeeds in Android Settings → Bluetooth
  without a PIN prompt. (Verified on production firmware after TASK-228.)
- **Linux / BlueZ** refuses Just Works when the peripheral requests MITM and returns
  `org.bluez.Error.AuthenticationFailed`. The pedal therefore cannot be paired from a
  Linux host via `bluetoothctl pair` or a GNOME/KDE BT applet.
- **Windows** behaviour is version-dependent; older builds refuse in the same way as
  BlueZ.

Commercial no-display BT keyboards (Logitech MX Keys, Apple Magic Keyboard, generic
compact keyboards, BT game controllers) ship with **MITM=false** — bonding + SC + Just
Works — which is the accepted posture for consumer HID peripherals that have no I/O to
support a MITM-capable pairing method. Our pedal should match.

## Fix to apply

Override the `BleKeyboard` library's default security posture in
`HookableBleKeyboard::onStarted()` (fires inside `BleKeyboard::begin()` after HID services
are registered but before advertising starts — see
[BLE_CONFIG_IMPLEMENTATION_NOTES.md](../../BLE_CONFIG_IMPLEMENTATION_NOTES.md) Challenge 2):

```cpp
void onStarted(BLEServer* pServer) override
{
    // Relax pairing policy: consumer no-display BT keyboards pair Just Works +
    // bonding + SC, not with MITM. MITM=true + NoInputNoOutput IOCap is a
    // mathematically unpairable combination on strict BT stacks (Linux BlueZ,
    // some Windows versions). See TASK-229.
    NimBLEDevice::setSecurityAuth(true, false, true);

    if (cb_)
        cb_(pServer);
}
```

## Acceptance Criteria

- [x] `HookableBleKeyboard::onStarted` sets `setSecurityAuth(true, false, true)` **and**
      `setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT)`. The IOCap override was necessary in
      addition to MITM=false — `BleKeyboard` leaves IOCap unset, letting NimBLE negotiate
      a pairing method that still required MITM confirmation. Forcing
      `NoInputNoOutput` on the peripheral side guarantees Just Works on every stack.
- [x] Production firmware rebuilt and flashed; `bluetoothctl pair 24:62:AB:D4:E0:D2`
      succeeds (no `AuthenticationFailed`) when the host runs a `NoInputNoOutput` agent
      (e.g. `agent NoInputNoOutput` in `bluetoothctl`).
- [x] Android pairing still succeeds without a PIN prompt (regression check).
- [ ] On-device BLE integration test (`test_ble_config_esp32`) — not re-run in this
      task; the test firmware does not use `BleKeyboard` so the change is invariant.
- [x] The production-path CLI upload (`pedal_config.py upload`) completes end-to-end on
      Linux after the pair step: pedal LEDs blink 3×, CLI prints `OK: upload successful`.

## Test Plan

**On-device tests** (`make test-esp32-ble-config`):

- Re-run the existing BLE config integration test suite. It runs with
  `BLE_CONFIG_TEST_BUILD` and initialises NimBLE directly without `BleKeyboard`, so the
  security-posture change should not affect it.

**Manual tests**:

- Android: forget any prior pairing, re-pair — confirm still works without a PIN.
- Linux: `bluetoothctl pair <addr>` — confirm no `AuthenticationFailed`.
- Linux CLI: `pedal_config.py upload data/profiles.json` on a paired host — confirm end
  to end success.

## Notes

- **Security implication**: MITM=false makes pairing vulnerable to active man-in-the-middle
  attacks *during the pairing ceremony* (a few seconds when the user first pairs a new
  host). Once bonded, SC-secured traffic is protected. This is the same posture as every
  commercial BT keyboard without a display; the trade-off is standard for consumer HID
  peripherals. See [BLE_CONFIG_IMPLEMENTATION_NOTES.md](../../BLE_CONFIG_IMPLEMENTATION_NOTES.md)
  Security considerations for the broader context.
- **Follow-up**: if an even lower-security path is needed for Config uploads specifically
  (the "Config-Only boot mode via BOOT button" idea), file that separately as an IDEA —
  it's a different problem class from this defect.
- **Sibling CLI fixes bundled in this close-out**:
  - `pedal_config.py` scan-filter: also match by name prefix `AwesomeStudioPe`, so the
    CLI finds the pedal on production firmware where the Config service UUID is in
    GATT but not in the advertisement.
  - `pedal_config.py` `write_gatt_char(..., response=True)`: switch from WRITE_NR to
    WRITE path (see [BLE_CONFIG_IMPLEMENTATION_NOTES.md](../../BLE_CONFIG_IMPLEMENTATION_NOTES.md)
    Challenge 4) — BlueZ's `AcquireWrite` backend for `response=False` fails before ATT
    MTU negotiation completes, producing `org.bluez.Error.Failed: Failed to initiate
    write`. The test runner already uses `response=True`; the CLI now matches.
- **UX gap observed during verification**: BlueZ auto-reconnects to the paired pedal
  as an HID keyboard immediately after a disconnect, which stops the pedal from
  advertising and makes subsequent CLI uploads fail scanning. Today the user must run
  `bluetoothctl disconnect <addr>` before each upload. Candidate follow-up: have the
  CLI auto-disconnect via D-Bus before scanning, or connect to the already-connected
  device by address without re-scanning.
