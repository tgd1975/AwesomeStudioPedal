import 'package:awesome_studio_pedal/screens/connected_pedal_screen.dart';
import 'package:awesome_studio_pedal/services/ble_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:provider/provider.dart';

@GenerateNiceMocks([MockSpec<BleService>()])
import 'connected_pedal_screen_test.mocks.dart';

Widget _wrap(BleService ble) => ChangeNotifierProvider<BleService>.value(
      value: ble,
      child: const MaterialApp(home: ConnectedPedalScreen()),
    );

void main() {
  testWidgets('connected: board row reflects readDeviceHardware result',
      (tester) async {
    final ble = MockBleService();
    when(ble.isConnected).thenReturn(true);
    when(ble.readDeviceHardware()).thenAnswer((_) async => 'esp32');
    when(ble.readDeviceFirmwareVersion()).thenAnswer((_) async => 'v0.4.1');

    await tester.pumpWidget(_wrap(ble));
    await tester.pumpAndSettle();

    expect(find.text('Board'), findsOneWidget);
    expect(find.text('esp32'), findsOneWidget);
  });

  testWidgets('connected: firmware row reflects readDeviceFirmwareVersion',
      (tester) async {
    final ble = MockBleService();
    when(ble.isConnected).thenReturn(true);
    when(ble.readDeviceHardware()).thenAnswer((_) async => 'esp32');
    when(ble.readDeviceFirmwareVersion()).thenAnswer((_) async => 'v0.4.1');

    await tester.pumpWidget(_wrap(ble));
    await tester.pumpAndSettle();

    expect(find.text('Firmware'), findsOneWidget);
    expect(find.text('v0.4.1'), findsOneWidget);
  });

  testWidgets('connected nRF52840 (no firmware char): firmware row stays placeholder',
      (tester) async {
    final ble = MockBleService();
    when(ble.isConnected).thenReturn(true);
    when(ble.readDeviceHardware()).thenAnswer((_) async => 'nrf52840');
    // Characteristic absent — readDeviceFirmwareVersion returns null.
    when(ble.readDeviceFirmwareVersion()).thenAnswer((_) async => null);

    await tester.pumpWidget(_wrap(ble));
    await tester.pumpAndSettle();

    expect(find.text('nrf52840'), findsOneWidget);
    // Firmware row falls back to the placeholder tooltip alongside
    // Configuration and Storage (3 placeholder rows total).
    expect(
      find.byTooltip('Available after firmware update'),
      findsNWidgets(3),
    );
  });

  testWidgets('disconnected: shows banner and "—" rows', (tester) async {
    final ble = MockBleService();
    when(ble.isConnected).thenReturn(false);
    when(ble.readDeviceHardware()).thenAnswer((_) async => null);
    when(ble.readDeviceFirmwareVersion()).thenAnswer((_) async => null);

    await tester.pumpWidget(_wrap(ble));
    await tester.pumpAndSettle();

    expect(find.textContaining('Not connected'), findsOneWidget);
    // Board row shows a literal em-dash; pending rows show
    // "— (Available after firmware update)".
    expect(find.text('—'), findsOneWidget);
  });

  testWidgets('placeholder rows render the pending-firmware tooltip',
      (tester) async {
    final ble = MockBleService();
    when(ble.isConnected).thenReturn(true);
    when(ble.readDeviceHardware()).thenAnswer((_) async => 'esp32');
    // No firmware char available — fall back to placeholder.
    when(ble.readDeviceFirmwareVersion()).thenAnswer((_) async => null);

    await tester.pumpWidget(_wrap(ble));
    await tester.pumpAndSettle();

    expect(find.text('Firmware'), findsOneWidget);
    expect(find.text('Configuration'), findsOneWidget);
    expect(find.text('Storage'), findsOneWidget);
    // The 3 placeholder rows are the pending-firmware Tooltips. Other
    // framework chrome may add tooltips too (e.g. AppBar BackButton).
    expect(
      find.byTooltip('Available after firmware update'),
      findsNWidgets(3),
    );
  });
}
