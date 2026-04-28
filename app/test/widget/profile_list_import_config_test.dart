import 'dart:io';

import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:flutter_test/flutter_test.dart';

/// TASK-267: the Profile List screen exposes an "Import Hardware Config"
/// action (Option A in the task body). The full picker → file-system →
/// validate → loadHardwareConfigFromJson chain depends on a platform
/// plugin and is exercised by the /verify-on-device UP-08 path. This
/// suite covers the parts that *can* be tested host-side:
///
/// 1. ProfilesState.loadHardwareConfigFromJson populates hardwareConfig
///    (the inner action the import flow ultimately calls).
/// 2. A static-source guard that ProfileListScreen wires an Import
///    Hardware Config menu item to a handler — preventing the screen
///    from regressing to the no-UI state described in the task.
void main() {
  test('loadHardwareConfigFromJson populates hardwareConfig', () {
    final state = ProfilesState();
    expect(state.hardwareConfig, isNull);
    state.loadHardwareConfigFromJson(const {
      'hardware': 'esp32',
      'numButtons': 2,
      'numProfiles': 4,
      'numSelectLeds': 2,
      'ledBluetooth': 26,
      'ledPower': 25,
      'ledSelect': [5, 18],
      'buttonSelect': 21,
      'buttonPins': [13, 12],
    });
    expect(state.hardwareConfig, isNotNull);
    expect(state.hardwareConfig!.hardware, equals('esp32'));
  });

  test('ProfileListScreen exposes an Import Hardware Config action', () {
    final src = File('lib/screens/profile_list_screen.dart').readAsStringSync();
    expect(src.contains('Import Hardware Config'), isTrue,
        reason: 'ProfileListScreen must expose an "Import Hardware Config" '
            'menu item — the only path for the user to populate '
            'state.hardwareConfig from disk. See TASK-267.');
    expect(src.contains('importHardwareConfig'), isTrue,
        reason: 'The menu item must route through '
            'FileService.importHardwareConfig (which validates against the '
            'config schema before loading). See TASK-267.');
  });
}
