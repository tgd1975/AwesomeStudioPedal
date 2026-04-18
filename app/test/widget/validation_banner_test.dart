import 'package:awesome_studio_pedal/models/profile.dart';
import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:awesome_studio_pedal/services/schema_service.dart';
import 'package:awesome_studio_pedal/widgets/validation_banner.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:provider/provider.dart';

@GenerateNiceMocks([MockSpec<SchemaService>()])
import 'validation_banner_test.mocks.dart';

void main() {
  testWidgets('ValidationBanner shows red error count when invalid profile is loaded',
      (tester) async {
    final schemaService = MockSchemaService();
    final state = ProfilesState();

    when(schemaService.validateProfiles(any)).thenAnswer(
      (_) async => ValidationResult.invalid(['Missing required field: type']),
    );

    await tester.pumpWidget(
      MultiProvider(
        providers: [
          ChangeNotifierProvider<ProfilesState>.value(value: state),
          Provider<SchemaService>.value(value: schemaService),
        ],
        child: const MaterialApp(home: Scaffold(body: ValidationBanner())),
      ),
    );

    state.addProfile(Profile(name: 'Test', buttons: {}));
    await tester.pumpAndSettle();

    expect(find.textContaining('error'), findsOneWidget);
  });
}
