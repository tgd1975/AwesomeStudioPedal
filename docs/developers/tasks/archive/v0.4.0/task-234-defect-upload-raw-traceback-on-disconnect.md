---
id: TASK-234
title: Defect — CLI upload shows raw Python traceback when BLE disconnects mid-transfer
status: closed
opened: 2026-04-23
closed: 2026-04-24
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: No
epic: feature_test
order: 22
---

## Description

Test plan `FEATURE_TEST_PLAN.md` §1.3 U-05 expects:

> Start upload, physically disconnect USB power from pedal mid-transfer →
> CLI reports connection loss clearly.

Reality: the CLI crashes with a full Python traceback. The raw BleakError
propagates out of `scripts/pedal_config.py::_upload` because no
`try/except BleakError` wraps the BLE calls (connect, start_notify,
write_gatt_char, stop_notify, read_gatt_char).

Reproduction (from TASK-151 U-05 execution):

```
Uploading 61 packets…
  10/61
  20/61
  30/61
Traceback (most recent call last):
  File "scripts/pedal_config.py", line 333, in <module>
    sys.exit(main())
  …
  File "scripts/pedal_config.py", line 245, in _upload
    await client.write_gatt_char(char_uuid, chunk, response=True)
  …
  File "bleak/backends/bluezdbus/utils.py", line 108, in assert_gatt_reply
    raise BleakGATTProtocolError(…)
bleak.exc.BleakGATTProtocolError: (<BleakGATTProtocolErrorCode.UNLIKELY_ERROR: 14>, 'GATT Protocol Error: Unlikely Error')
```

`UNLIKELY_ERROR` is what BlueZ maps an abrupt peer disconnect to at the
GATT layer. The chunk-write loop also does not handle disconnects between
writes.

## Fix scope

1. Wrap all BLE calls in `_upload` with a narrow `try/except BleakError`
   (connect, start_notify, the chunk-write loop, status-wait, stop_notify,
   read_gatt_char on the hardware-identity char).
2. On a caught error, print a human-readable message such as
   `ERROR: connection to pedal lost during upload (<exception summary>).
   Check that the pedal is powered and within range, then retry.` and
   return exit code 1 — matching the pattern already landed in
   `_scan` via TASK-227.
3. Also catch `asyncio.TimeoutError` if any underlying bleak call can
   time out; today the status-wait uses a 10s polling loop that already
   handles its own "no notification received" path, but a disconnected
   client will instead throw on `stop_notify` and that should be clean too.

## Acceptance Criteria

- [ ] `_upload` catches `BleakError` (and any subclass bleak raises on
      disconnect, at minimum `BleakGATTProtocolError` and
      `BleakDBusError`) around every BLE call it makes.
- [ ] Simulating disconnect mid-transfer (e.g. by monkeypatching
      `write_gatt_char` to raise `BleakError`) in the unit tests makes
      `_upload` return 1 and print a clean, non-traceback error.
- [ ] Manual re-run of FEATURE_TEST_PLAN.md §1.3 U-05: upload starts,
      USB yanked mid-transfer, CLI prints one line about connection
      loss and exits 1.

## Test Plan

**Host tests** (`scripts/tests/test_pedal_config.py`):

- Add cases that monkeypatch `BleakClient` and its methods to raise
  `BleakError`/`BleakGATTProtocolError` at each of:
  (a) `__aenter__` (connect failure);
  (b) `start_notify`;
  (c) `write_gatt_char` mid-chunk-loop (the interesting case for U-05);
  (d) `stop_notify`.
  In each case assert `_upload` returns 1, prints an ERROR line to
  stderr, and does NOT print a Python traceback.

No on-device tests — this is CLI-side error handling.

## Notes

- Surfaced during TASK-151 U-05 feature test execution. U-05 is now
  **FAIL** in FEATURE_TEST_PLAN.md until this lands.
- Sibling to TASK-227, which fixed the same class of bug for the scan
  path. The same pattern (`try: … except BleakError as exc: print…
  return 1`) applies here.
