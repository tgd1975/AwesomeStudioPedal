---
id: TASK-261
title: Defect — Upload chunk size (510 B) exceeds Android writeWithoutResponse MTU cap (252 B)
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (2-4h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 39
prerequisites: [TASK-233, TASK-240]
---

## Description

Discovered while running TASK-156 UP-03 on 2026-04-26 (Pixel 9, Android 16).

Tapping **Upload Profiles** on the Upload screen always fails on
Android. The first chunk write throws:

```
PlatformException(writeCharacteristic, data longer than allowed.
dataLen: 512 > max: 252 (withoutResponse), null, null)
```

The chunk size is hard-coded in
[app/lib/services/ble_service.dart:175](app/lib/services/ble_service.dart#L175):

```dart
const chunkSize = 510;
// ... 2-byte big-endian sequence header is prepended ...
await characteristic.write([...seq, ...payload], withoutResponse: true);
```

So each write attempts `seq (2 B) + payload (510 B) = 512 B`. Android's
GATT layer caps `writeWithoutResponse` payloads at `MTU - 3`. With the
current default (no explicit `requestMtu` from the app), Android
negotiates MTU=255, which gives a 252 B payload cap. 512 > 252 → reject
on the first chunk every time.

Net effect: the upload feature has never worked on Android in this
build, regardless of profile size — even a one-profile JSON exceeds
the cap because the fixed chunk is always 510 B.

## Reproducer

1. Pixel 9 / Android 16, app post-TASK-258 build, pedal flashed and
   advertising (build verified working in TASK-153 round 2).
2. Connect to pedal from the in-app scanner.
3. Edit profiles → Import JSON → import `data/profiles.json` (the
   default 7-profile fixture).
4. Back to Home → Upload → tap **Upload Profiles**.
5. Observe: progress bar simulation completes ("Uploading… chunk 14 / 14"
   shown for ~½ s) but the screen never advances. No SnackBar, no
   error dialog. Logcat shows the `PlatformException` above. UI hangs
   forever (button does not re-enable).

(The hang is the symptom of TASK-266 — the swallowed exception. The
chunk-size cap is the underlying defect this task fixes.)

## Acceptance Criteria

- [x] `BleService._upload` chunks at a size that is safe for Android's
      writeWithoutResponse cap on every supported MTU. Concretely:
      either request MTU=517 explicitly via
      `BluetoothDevice.requestMtu(517)` post-connect and then chunk at
      `mtu - 3 - 2` (subtracting the 2-byte seq header), or chunk at a
      conservative fixed 180 B that is safe for the worst-case MTU=23
      / 20-byte payload, plus iOS safety margin.
- [x] Firmware reassembly side handles the new chunk size unchanged
      — sequence header semantics do not change.
- [x] Unit-level coverage: a test (or static-source guard, à la
      `ble_service_scan_filter_test.dart`) that the chunk-size constant
      is `<= 250 B` (a tight upper bound that catches a regression to
      the 510 B value).
- [x] On-device verification: re-run TASK-156 UP-03 on Pixel 9 — the
      profile JSON uploads cleanly, the SnackBar "Upload successful!"
      shows, and a profile reload on the pedal confirms the JSON
      arrived intact.

## Test Plan

**Static-source guard** (`flutter test`) — fastest signal:

```dart
test('upload chunk size is below android writeWithoutResponse cap', () {
  final src = File('lib/services/ble_service.dart').readAsStringSync();
  final m = RegExp(r"chunkSize\s*=\s*(\d+)").firstMatch(src);
  expect(m, isNotNull, reason: 'chunkSize constant not found');
  expect(int.parse(m!.group(1)!), lessThanOrEqualTo(250),
      reason: 'chunk + 2-byte seq header must fit Android MTU=255 cap (252 B)');
});
```

**Manual on-device:** re-run TASK-156 UP-03 with the default 7-profile
fixture; observe success SnackBar within 5 s.

## Notes

- The Android limit is `MTU - 3`. iOS allows up to `mtu - 3` for
  writeWithoutResponse and `mtu - 5` for write-with-response — so the
  cap that matters is the Android one in practice.
- Requesting a larger MTU is the cleanest fix and matches the firmware
  side, which is configured for MTU=517 in NimBLE
  ([lib/hardware/esp32/src/ble_config_service.cpp](../../../../lib/hardware/esp32/src/ble_config_service.cpp)).
  Without an explicit `requestMtu` on Android, the negotiated MTU is
  whatever the central asks for — which on Android 12+ defaults to 23
  unless the app or the BlueZ-equivalent driver bumps it.
- Cross-reference: TASK-266 (this round) — the swallowed
  `PlatformException` that hides this failure from the user.
- Surfaced in TASK-156 UP-03 round-2 on 2026-04-26.
- Verified on-device on 2026-04-26 via `/verify-on-device TASK-261 UP-03`
  on Pixel 9 / Android 16. Tap **Upload Profiles** → "Upload successful!"
  SnackBar within 2 s, no `PlatformException` in logcat. Screenshot:
  `/tmp/verify_TASK-261_UP-03_success.png`.
