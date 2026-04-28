---
id: TASK-246
title: Defect — pairing_pin advertises PIN protection but pairs Just-Works (MITM bit never set)
status: closed
opened: 2026-04-25
closed: 2026-04-25
effort: Small (<2h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1
complexity: Small
human-in-loop: Verify
epic: feature_test
order: 26
prerequisites: [TASK-237]
---

## Description

`pairing_pin` in `data/config.json` is intended to gate BLE bonding behind a
6-digit passkey: a non-null integer should require the connecting host to
enter that PIN before bonding succeeds. TASK-237 wired the config field, the
NimBLE passkey, and `BLE_HS_IO_DISPLAY_ONLY` IOCap.

But the firmware also sets `setSecurityAuth(bonding=true, mitm=false, sc=true)`
unconditionally and never raises the MITM bit when `pairingEnabled` is true.
With MITM=false on the responder, the SMP IOCap matrix collapses to **Just
Works** regardless of the IOCap value, so any host in range bonds silently
without ever being asked for the PIN. The `pairing_pin` value is configured
in NimBLE but never exercised.

Discovered during TASK-238 manual pairing smoke test on 2026-04-25: with
`data/config.json` carrying `pairing_pin: 12345`, both sides freshly cleaned
(BlueZ `remove`, NVS `erase_region 0x9000 0x5000`), and a `KeyboardOnly`
agent registered on BlueZ, `bluetoothctl pair 24:62:AB:D4:E0:D2` reported
"Pairing successful" in ~5 seconds with **no passkey prompt at all**.
`bluetoothctl info` afterwards confirmed `Paired: yes, Bonded: yes`.

### Location

[lib/hardware/esp32/include/ble_keyboard_adapter.h](../../../../lib/hardware/esp32/include/ble_keyboard_adapter.h):

```cpp
void onStarted(BLEServer* pServer) override
{
    NimBLEDevice::setSecurityAuth(/*bonding=*/true, /*mitm=*/false, /*sc=*/true);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

    if (hardwareConfig.pairingEnabled)
    {
        NimBLEDevice::setSecurityPasskey(hardwareConfig.pairingPin);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
        // MISSING: setSecurityAuth(true, true, true) to raise the MITM bit
    }

    if (cb_) cb_(pServer);
}
```

The matching nRF52840 adapter likely has the same shape — verify and fix in
both places.

### Why "just call setSecurityAuth again with mitm=true" needs care

The TASK-229 comment above line 50 documents that MITM=false /
IOCap=NoInputNoOutput is what makes pairing work with BlueZ for the
no-passkey case (test fixtures, casual phone pairing). The fix must:

- Keep the MITM=false / NoInputNoOutput path for `pairingEnabled == false`
  (so test fixtures and the existing `pairing_pin: null` flow still work).
- Raise MITM=true *only* when `pairingEnabled == true`, alongside the
  DisplayOnly IOCap and the configured passkey.

The two paths should be obviously mutually exclusive in the code so a future
reader doesn't reintroduce the same defect.

## Acceptance Criteria

- [x] With `pairing_pin: 12345` in `data/config.json`, a fresh
      `bluetoothctl pair` (after `remove` + NVS erase) is **rejected**
      unless the agent supplies passkey `012345`.
- [x] With the right passkey, pairing completes and the bond is reused on
      reconnect (no second prompt).
- [x] With `pairing_pin: null` or absent, pairing still completes
      Just-Works style (the behaviour the BLE config integration test relies
      on — verify by re-running `make test-esp32-ble-config`).
- nRF52840 parity is **out of scope** here and tracked in
  **[TASK-249](task-249-nrf52840-pairing-pin-unwired.md)**: that adapter
  doesn't even read `hardwareConfig.pairingPin`, so the fix shape is
  different and the target is currently "implemented, not tested" anyway.

## Test Plan

**On-device manual test** (Linux + BlueZ):

1. Erase host bond: `bluetoothctl remove 24:62:AB:D4:E0:D2`
2. Erase pedal NVS: `python3 ~/.platformio/packages/tool-esptoolpy/esptool.py --port /dev/ttyUSB0 erase_region 0x9000 0x5000`
3. Reflash production firmware + filesystem (`make upload-esp32 && make uploadfs-esp32`)
4. `bluetoothctl` → `agent KeyboardOnly` → `default-agent` → `pair 24:62:AB:D4:E0:D2`
5. Assert: passkey prompt appears; entering `012345` succeeds; entering
   anything else fails with `org.bluez.Error.AuthenticationFailed` or
   equivalent.
6. Disconnect + reconnect; assert no second prompt.
7. Repeat with `pairing_pin: null` fixture; assert no prompt at all and the
   `make test-esp32-ble-config` runner still passes.

**Regression coverage**: this is hard to host-test without a real BLE stack.
At minimum, add a check that `setSecurityAuth` is called with `mitm=true`
when `pairingEnabled` is set — a fake-NimBLE host shim could capture the
calls. Defer if too expensive; the on-device manual test above is the
authoritative gate.

## Notes

- Blocks TASK-238 acceptance criterion 2 (manual pairing smoke test). With
  this defect open, TASK-238 is closed against criteria 1 and 3 only.
- TASK-229 is the source of the existing MITM=false / NoInputNoOutput line
  and explains why we need a Just-Works path at all (BlueZ refuses MITM
  pairing if the responder advertises a capability it cannot satisfy). The
  fix must not regress the no-PIN flow that TASK-229 stabilised.
- Discovery commands and the cleanup recipe used during the smoke test are
  preserved in this task description so a future verifier doesn't have to
  reverse-engineer them.
