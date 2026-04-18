import 'package:awesome_studio_pedal/app.dart';
import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:awesome_studio_pedal/services/ble_service.dart';
import 'package:awesome_studio_pedal/services/file_service.dart';
import 'package:awesome_studio_pedal/services/schema_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:provider/provider.dart';

@GenerateNiceMocks([MockSpec<BleService>()])
import 'app_flow_test.mocks.dart';

void main() {
  group('App integration flow', () {
    late MockBleService mockBle;

    setUp(() {
      mockBle = MockBleService();
      when(mockBle.isConnected).thenReturn(false);
      when(mockBle.lastError).thenReturn(null);
      when(mockBle.scan()).thenAnswer((_) async => []);
    });

    Widget buildApp() {
      return MultiProvider(
        providers: [
          ChangeNotifierProvider<BleService>.value(value: mockBle),
          ChangeNotifierProvider(create: (_) => ProfilesState()),
          Provider(create: (_) => SchemaService()),
          ChangeNotifierProvider(create: (_) => FileService()),
        ],
        child: const App(),
      );
    }

    testWidgets('App launches and HomeScreen is shown', (tester) async {
      await tester.pumpWidget(buildApp());
      await tester.pumpAndSettle();
      expect(find.text('AwesomeStudioPedal'), findsOneWidget);
      expect(find.text('Connect to pedal'), findsOneWidget);
    });

    testWidgets('Tapping Connect to pedal navigates to ScannerScreen', (tester) async {
      await tester.pumpWidget(buildApp());
      await tester.pumpAndSettle();
      await tester.tap(find.text('Connect to pedal'));
      await tester.pumpAndSettle();
      expect(find.text('Connect to Pedal'), findsOneWidget);
    });

    testWidgets('Tapping Edit profiles navigates to ProfileListScreen', (tester) async {
      await tester.pumpWidget(buildApp());
      await tester.pumpAndSettle();
      await tester.tap(find.text('Edit profiles'));
      await tester.pumpAndSettle();
      expect(find.text('Profiles'), findsOneWidget);
    });
  });
}
