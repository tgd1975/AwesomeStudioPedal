import 'dart:io';

import 'package:flutter_test/flutter_test.dart';

/// Static-source guard: the BleService scan must NOT use a withServices: UUID
/// filter. See TASK-258 and KNOWN_ISSUES.md — the firmware advertises only the
/// HID UUID (0x1812), not our 128-bit config UUID, so a UUID filter would
/// reject the pedal entirely. Re-introducing the filter would bring the
/// "in-app scan returns nothing" defect back, and there is no end-to-end
/// device test in CI that would catch it.
void main() {
  test('BleService.scan does not filter startScan by service UUID', () {
    // Strip line comments so we only assert against actual code, not the
    // explanatory comment that intentionally mentions `withServices:`.
    final source = File('lib/services/ble_service.dart')
        .readAsStringSync()
        .split('\n')
        .where((l) => !l.trimLeft().startsWith('//'))
        .join('\n');
    expect(
      source.contains('withServices:'),
      isFalse,
      reason: 'BleService scan must use unfiltered startScan + client-side '
          'name match. See TASK-258 / KNOWN_ISSUES.md.',
    );
  });

  test('BleService scans and matches by pedal name prefix', () {
    final source = File('lib/services/ble_service.dart').readAsStringSync();
    expect(source.contains('kPedalNamePrefix'), isTrue,
        reason: 'Scan results must be filtered by pedal name prefix.');
  });

  test('BleService.scan forces legacy advertising scan on Android', () {
    final source = File('lib/services/ble_service.dart').readAsStringSync();
    expect(source.contains('androidLegacy: true'), isTrue,
        reason: 'Without androidLegacy: true the Android 14+ extended-'
            'advertising scan misses the pedal entirely (NimBLE only does '
            'legacy 1M-PHY advertising). See TASK-258.');
  });

  test('BleService.scan checks adapterStateNow before starting the scan', () {
    final source = File('lib/services/ble_service.dart').readAsStringSync();
    expect(source.contains('adapterStateNow'), isTrue,
        reason: 'BleService.scan must short-circuit when Bluetooth is off — '
            'FlutterBluePlus.startScan does not throw in that state, so the '
            'caller would see an indefinite spinner. See TASK-262.');
    expect(source.contains("Bluetooth is off"), isTrue,
        reason: 'The off-state error message must contain "Bluetooth is off" '
            'so the scanner UI can match it and render the dedicated card. '
            'See TASK-262.');
  });

  test('BleService.scan listens for asynchronous scan errors', () {
    final source = File('lib/services/ble_service.dart').readAsStringSync();
    expect(source.contains('FlutterBluePlus.scanResults.listen'), isTrue,
        reason: 'BleService.scan must subscribe to scanResults to surface '
            'asynchronous failures (permission denied, adapter turned off '
            'mid-scan). Without it the platform pushes the error through '
            'addError but the caller never observes it. See TASK-262.');
  });

  test('BleService.scan checks runtime BLE permissions before scanning', () {
    final source = File('lib/services/ble_service.dart').readAsStringSync();
    expect(source.contains('hasBlePermissions'), isTrue,
        reason: 'BleService.scan must call hasBlePermissions() — on '
            'Android 12+, FlutterBluePlus.startScan does not throw when '
            'BLUETOOTH_SCAN/BLUETOOTH_CONNECT are denied; the caller would '
            'see an indefinite spinner. See TASK-262.');
    expect(source.contains('Bluetooth permission denied'), isTrue,
        reason: 'The permission-denied error message must contain '
            '"Bluetooth permission denied" so the scanner UI can match it '
            'and render the dedicated card. See TASK-262.');
  });
}
