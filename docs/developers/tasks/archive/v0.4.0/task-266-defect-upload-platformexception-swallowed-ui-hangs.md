---
id: TASK-266
title: Defect — Upload Profiles swallows non-FlutterBluePlusException errors; UI hangs at "chunk N/N" with no SnackBar or dialog
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (2-4h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: Main
epic: feature_test
order: 37
---

## Description

Discovered while running TASK-156 UP-03 on 2026-04-26 (Pixel 9, Android 16).

When `BleService._upload`'s `await characteristic.write(...)` throws a
`PlatformException` (the actual Android failure mode — see TASK-261 for
the underlying chunk-size bug), the exception is **not** caught:

[app/lib/services/ble_service.dart:173-195](app/lib/services/ble_service.dart#L173-L195):

```dart
try {
  // ...for each chunk: await characteristic.write(..., withoutResponse: true);
} on FlutterBluePlusException catch (e) {
  await sub.cancel();
  return UploadResult.failure(e.description ?? e.toString());
}
```

The `on FlutterBluePlusException` clause only catches that specific
type. The platform-channel layer in `flutter_blue_plus_android` actually
throws a `PlatformException` (`writeCharacteristic, data longer than
allowed. dataLen: 512 > max: 252 (withoutResponse)`), which slips past
the catch.

Caller-side consequence
([app/lib/screens/upload_screen.dart:40-76](app/lib/screens/upload_screen.dart#L40-L76)):

- The `setState(() { _uploading = true; })` already fired.
- The simulated chunk-progress `for` loop already ran to 100 %, so the
  UI shows `Uploading… chunk 14 / 14` and a full progress bar.
- `await ble.uploadProfiles(json)` never returns (uncaught exception
  propagates up the async chain and is delivered as an unhandled
  Flutter error to the zone).
- The `if (result.success) … else _showError(...)` branch is never
  reached; no SnackBar, no error dialog.
- The upload button stays enabled (we already returned from
  `_uploadProfiles`'s post-await `setState`'s reach), but
  `_uploading` is also still `true` because the cleanup `setState`
  never ran. The screen looks frozen.
- Worse: the `StreamSubscription` on the status characteristic
  (`_statusChar!.onValueReceived`) is never cancelled, so it keeps
  listening across subsequent navigations.

Net effect: an upload that fails for any reason other than a
`FlutterBluePlusException` (which is essentially every failure path
on Android, since flutter_blue_plus_android translates its errors
to PlatformException) **looks identical to a successful in-progress
upload that just happens to be taking forever.** The user is given
no signal that anything is wrong.

## Reproducer

Easiest path: trigger TASK-261 (chunk too large for MTU) which is the
defect that surfaced this:

1. Pixel 9 / Android 16, app post-TASK-258 build.
2. Connect to pedal, import `data/profiles.json`.
3. Tap **Upload Profiles**.
4. Observe: progress bar fills to 100 %, "Uploading… chunk 14 / 14"
   shown, then nothing. No error UI. Logcat shows the unhandled
   `PlatformException`. Backing out and re-entering the screen leaves
   the previous progress visible until first re-render.

## Acceptance Criteria

- [x] `BleService._upload` catches all exceptions during chunk writes
      (broaden `on FlutterBluePlusException` to also catch
      `PlatformException` — or use a bare `catch (e)`), cancels the
      `StreamSubscription`, and returns `UploadResult.failure` with a
      user-readable reason.
- [x] `_UploadScreenState._uploadProfiles` resets `_uploading` /
      `_progress` in a `finally` block so the UI never stays stuck on
      the progress state if anything throws.
- [x] Same hardening applied to `_uploadConfig`
      ([upload_screen.dart:78-114](app/lib/screens/upload_screen.dart#L78-L114))
      — same swallow-then-hang pattern is present there too.
- [x] Coverage: a unit test that wraps `BleService._upload` (or its
      moral equivalent) and asserts that when the underlying
      `characteristic.write` throws `PlatformException`, the function
      returns `UploadResult.failure(...)` rather than re-throws — no
      hardware needed (mock characteristic).

## Test Plan

**Host (Flutter unit) test:**

```dart
test('upload returns failure on PlatformException, never re-throws', () async {
  final char = MockBluetoothCharacteristic();
  when(char.write(any, withoutResponse: anyNamed('withoutResponse')))
      .thenThrow(PlatformException(code: 'writeCharacteristic',
          message: 'data longer than allowed. dataLen: 512 > max: 252'));
  final result = await fakeBleService.uploadProfiles('{"profiles":[]}');
  expect(result.success, isFalse);
  expect(result.errorMessage, contains('data longer than allowed'));
});
```

**Manual on-device:** re-run TASK-156 UP-03; with TASK-261 unfixed it
should now show an "Upload Failed" dialog with the platform error
message instead of hanging. Once TASK-261 is also fixed, both the
happy and failure paths should behave correctly.

## Notes

- This is a defect-in-defect-handling — even after TASK-261 lands, the
  current swallow-then-hang shape is wrong: any future GATT-level error
  (disconnect mid-write, MTU change mid-write, busy queue) would
  reproduce this exact hang.
- `_uploadConfig` has the same defect *plus* a duplicated `if (!mounted)
  return;` at lines 91 and 91 area; clean both up while in there.
- Cross-reference: TASK-261 (the underlying chunk-size bug that made
  this discoverable).
- Surfaced in TASK-156 UP-03 round-2 on 2026-04-26.
- Verified on-device on 2026-04-26 via `/verify-on-device` on Pixel 9 /
  Android 16. Happy path (UP-03): Upload Profiles → "Upload successful!"
  SnackBar in 2 s, no leaked exception. Failure path: with `adb shell
  svc bluetooth disable` fired ~0.5 s after tapping Upload Profiles, the
  screen showed an **Upload Failed** dialog with reason "Not connected
  or characteristic unavailable" (no hang, progress UI cleared).
  Screenshots: `/tmp/verify_TASK-266_UP-03_success.png`,
  `/tmp/verify_TASK-266_failure_path.png`.
