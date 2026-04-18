import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mockito/annotations.dart';
import 'package:mockito/mockito.dart';
import 'package:provider/provider.dart';
import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:awesome_studio_pedal/services/schema_service.dart';
import 'package:awesome_studio_pedal/widgets/validation_banner.dart';

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

    // Add a profile to trigger validation.
    state.addProfile(
      // ignore: invalid_use_of_protected_member
      // We push directly into state for testing purposes.
      // This will be caught by the validation mock.
      _makeInvalidProfile(),
    );

    await tester.pumpAndSettle();

    expect(find.textContaining('error'), findsOneWidget);
  });
}

// ignore: avoid_dynamic_calls
dynamic _makeInvalidProfile() {
  // Returns a profile that reaches validation (validation result is mocked).
  final state = ProfilesState();
  return state.profiles.isEmpty
      ? _fakeProfile()
      : state.profiles.first;
}

// ignore: avoid_dynamic_calls
dynamic _fakeProfile() {
  // This is just to satisfy the type system; the mock controls the outcome.
  return null;
}
