import 'dart:io';

import 'package:flutter_test/flutter_test.dart';

/// Static-source guard: BleService._upload must chunk at a size that fits
/// Android's writeWithoutResponse cap (MTU - 3) on the default-negotiated
/// MTU=255 → 252 B payload. The previous 510 B value caused every Android
/// upload to fail with `data longer than allowed` on the first chunk.
/// See TASK-261.
void main() {
  test('upload chunk size is below android writeWithoutResponse cap', () {
    final src = File('lib/services/ble_service.dart').readAsStringSync();
    final m = RegExp(r'chunkSize\s*=\s*(\d+)').firstMatch(src);
    expect(m, isNotNull,
        reason: 'chunkSize constant not found in ble_service.dart');
    final value = int.parse(m!.group(1)!);
    expect(value, lessThanOrEqualTo(250),
        reason: 'chunk + 2-byte seq header must fit Android MTU=255 cap '
            '(252 B). See TASK-261.');
  });
}
