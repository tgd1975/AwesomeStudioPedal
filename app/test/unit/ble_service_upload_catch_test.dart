import 'dart:io';

import 'package:flutter_test/flutter_test.dart';

/// Static-source guard: BleService._upload must catch *all* exceptions during
/// chunk writes, not just FlutterBluePlusException. The Android plugin
/// (flutter_blue_plus_android) throws PlatformException for GATT-level
/// failures (chunk too large, disconnect mid-write, MTU change), and a
/// narrow `on FlutterBluePlusException catch` would let that exception
/// escape — leaking the StreamSubscription and leaving the caller's
/// progress UI hung at "chunk N/N" forever. See TASK-266.
void main() {
  test('upload chunk-write try block has a broad catch (not just FBPException)',
      () {
    final src = File('lib/services/ble_service.dart').readAsStringSync();
    // The fragile-but-fast check: somewhere in the file, there is a bare
    // `catch (` or `catch(`. The narrow-catch regression looked like
    // `} on FlutterBluePlusException catch (e) {` with no fallthrough.
    final hasBareCatch = RegExp(r'\}\s*catch\s*\(').hasMatch(src);
    expect(hasBareCatch, isTrue,
        reason: 'BleService._upload must include a bare `catch (e)` so that '
            'PlatformException (thrown by flutter_blue_plus_android) is '
            'caught and converted to UploadResult.failure. See TASK-266.');
  });
}
