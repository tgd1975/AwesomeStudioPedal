---
id: TASK-227
title: Defect — CLI scan broken on bleak ≥ 3.0 and on disabled BLE adapter
status: closed
closed: 2026-04-23
opened: 2026-04-23
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: Support
epic: feature_test
order: 15
---

## Description

Two defects were found and fixed inline during the TASK-150 feature test of the
`scan` subcommand in `scripts/pedal_config.py`:

1. **bleak 3.x compatibility** — the scan path accessed `d.metadata["uuids"]`,
   which does not exist on bleak ≥ 3.0. Any fresh install of `requirements.txt`
   (which does not pin bleak) would crash with an `AttributeError` on every scan.
   Fix: switched to `BleakScanner.discover(timeout=5.0, return_adv=True)` and read
   `service_uuids` / `rssi` from the returned `AdvertisementData`.

2. **BLE adapter disabled surfaces as a raw Python traceback** — when the host
   machine's Bluetooth is off, bleak raises `BleakBluetoothNotAvailableError`,
   which was not caught. Fix: catch `BleakError` around the `discover` call and
   print a human-readable message (plus hint "Is Bluetooth enabled on this
   machine?") and exit 1.

Both fixes are already applied in [scripts/pedal_config.py](../../../../scripts/pedal_config.py)
(function `_scan`, lines ~132–160). This task covers the follow-up work to
harden the fix and prevent regression.

## Acceptance Criteria

- [x] `requirements.txt` pins bleak to `>=0.20,<4`.
- [x] `scripts/tests/test_pedal_config.py` has four new mocked tests for
      `_scan`: happy-path (pedal tagged, non-pedal not tagged), no-pedal
      message, empty-results message, and `BleakError` branch (clean exit 1,
      no traceback). All 41 tests pass.
- [x] S-01 re-verified manually post-change (pedal tagged, RSSI=-46).
      S-02/S-03/CU-03 covered by the new unit tests — CLI source was untouched
      in this task so the manual branches would reproduce TASK-150 results.

## Test Plan

**Host tests** (Python, `scripts/tests/`):

- Add a test that monkeypatches `bleak.BleakScanner.discover` to return a
  mocked `{BLEDevice: (BLEDevice, AdvertisementData)}` dict and asserts the
  `← pedal` marker appears for a device whose `service_uuids` contain
  `SERVICE_UUID`, and does not appear otherwise.
- Add a test that monkeypatches `discover` to raise `BleakError("adapter off")`
  and asserts `_scan` returns `1` and prints a non-traceback error.

No on-device tests required — this is CLI-side behaviour independent of
firmware.

## Notes

- The truncated device name in the adv ("AwesomeStudioPe" instead of
  "AwesomeStudioPedal") is a separate, cosmetic observation surfaced during
  S-01. It is **not** in scope for this defect — file separately if we want to
  act on it.
- Consider whether the CLI should also print a friendlier wrapper around the
  `BleakError` repr (`('No powered Bluetooth adapters found…', <…POWERED_OFF:
  3>)`) — currently it passes the raw `exc` through. Good-enough for now.
