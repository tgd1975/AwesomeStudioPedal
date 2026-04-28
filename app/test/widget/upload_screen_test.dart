import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:awesome_studio_pedal/models/upload_result.dart';
import 'package:awesome_studio_pedal/screens/upload_screen.dart';
import 'package:awesome_studio_pedal/services/ble_service.dart';
import 'package:awesome_studio_pedal/services/schema_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:provider/provider.dart';

@GenerateNiceMocks([MockSpec<BleService>(), MockSpec<SchemaService>()])
import 'upload_screen_test.mocks.dart';

final _kHwJson = {
  'hardware': 'esp32',
  'numButtons': 2,
  'numProfiles': 4,
  'numSelectLeds': 2,
  'ledBluetooth': 26,
  'ledPower': 25,
  'ledSelect': [5, 18],
  'buttonSelect': 21,
  'buttonPins': [13, 12],
};

Widget _buildScreen({
  required BleService ble,
  required SchemaService schema,
  required ProfilesState state,
}) {
  return MultiProvider(
    providers: [
      ChangeNotifierProvider<ProfilesState>.value(value: state),
      ChangeNotifierProvider<BleService>.value(value: ble),
      Provider<SchemaService>.value(value: schema),
    ],
    child: const MaterialApp(home: UploadScreen()),
  );
}

void main() {
  late MockBleService ble;
  late MockSchemaService schema;
  late ProfilesState state;

  setUp(() {
    ble = MockBleService();
    schema = MockSchemaService();
    state = ProfilesState();
    state.loadHardwareConfigFromJson(_kHwJson);

    when(ble.isConnected).thenReturn(true);
    when(schema.validateProfiles(any))
        .thenAnswer((_) async => ValidationResult.valid());
  });

  testWidgets('hardware mismatch shows error dialog and blocks upload',
      (tester) async {
    when(ble.readDeviceHardware()).thenAnswer((_) async => 'nrf52840');

    await tester
        .pumpWidget(_buildScreen(ble: ble, schema: schema, state: state));
    await tester.pumpAndSettle();

    await tester.tap(find.text('Upload Hardware Config'));
    await tester.pumpAndSettle();

    expect(find.text('Upload Failed'), findsOneWidget);
    expect(find.textContaining('Hardware mismatch'), findsOneWidget);
    verifyNever(ble.uploadConfig(any));
  });

  testWidgets('matching hardware proceeds to upload', (tester) async {
    when(ble.readDeviceHardware()).thenAnswer((_) async => 'esp32');
    when(ble.uploadConfig(any))
        .thenAnswer((_) async => const UploadResult.success());

    await tester
        .pumpWidget(_buildScreen(ble: ble, schema: schema, state: state));
    await tester.pumpAndSettle();

    await tester.tap(find.text('Upload Hardware Config'));
    await tester.pumpAndSettle();

    verify(ble.uploadConfig(any)).called(1);
    expect(find.text('Upload Failed'), findsNothing);
  });

  testWidgets('old firmware (null hardware) shows warning snackbar not dialog',
      (tester) async {
    when(ble.readDeviceHardware()).thenAnswer((_) async => null);
    when(ble.uploadConfig(any))
        .thenAnswer((_) async => const UploadResult.success());

    await tester
        .pumpWidget(_buildScreen(ble: ble, schema: schema, state: state));
    await tester.pumpAndSettle();

    await tester.tap(find.text('Upload Hardware Config'));
    await tester.pumpAndSettle();

    verify(ble.uploadConfig(any)).called(1);
    expect(find.text('Upload Failed'), findsNothing);
  });

  // TASK-266: a failed upload must surface as a dialog and the progress UI
  // must reset — previously the on FlutterBluePlusException catch let
  // PlatformException escape and the screen hung at "Uploading… chunk N/N".
  testWidgets('upload failure shows error dialog and clears progress UI',
      (tester) async {
    when(ble.uploadProfiles(any)).thenAnswer((_) async =>
        const UploadResult.failure('data longer than allowed. dataLen: 512'));

    await tester
        .pumpWidget(_buildScreen(ble: ble, schema: schema, state: state));
    await tester.pumpAndSettle();

    await tester.tap(find.text('Upload Profiles'));
    await tester.pumpAndSettle();

    expect(find.text('Upload Failed'), findsOneWidget);
    expect(find.textContaining('data longer than allowed'), findsOneWidget);
    expect(find.textContaining('Uploading…'), findsNothing);
  });
}
