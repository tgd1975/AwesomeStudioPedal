import 'package:awesome_studio_pedal/models/action_config.dart';
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
  testWidgets(
      'ValidationBanner shows red error count when invalid profile is loaded',
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

  // Regression guard for TASK-281: editing an action's value without
  // changing the profile count must trigger revalidation.
  testWidgets(
      'ValidationBanner refreshes when an action is edited (count unchanged)',
      (tester) async {
    final schemaService = MockSchemaService();
    final state = ProfilesState();

    // First validation call (after the bad action is loaded) returns 1 error.
    // Subsequent calls (after the fix) return valid.
    var callCount = 0;
    when(schemaService.validateProfiles(any)).thenAnswer((_) async {
      callCount++;
      if (callCount == 1) {
        return ValidationResult.invalid(['Unresolvable action value']);
      }
      return ValidationResult.valid();
    });

    state.addProfile(Profile(
      name: 'P1',
      buttons: {
        'btn1': ActionConfig(type: 'KeyAction', value: 'NOT_A_VALID_KEY'),
      },
    ));

    await tester.pumpWidget(
      MultiProvider(
        providers: [
          ChangeNotifierProvider<ProfilesState>.value(value: state),
          Provider<SchemaService>.value(value: schemaService),
        ],
        child: const MaterialApp(home: Scaffold(body: ValidationBanner())),
      ),
    );
    await tester.pumpAndSettle();

    expect(find.textContaining('error'), findsOneWidget);
    expect(find.text('Valid ✓'), findsNothing);

    // Fix the action value in place — profile count is unchanged.
    final fixed = state.profiles.first.copyWith(buttons: {
      'btn1': ActionConfig(type: 'KeyAction', value: 'KEY_PAGE_UP'),
    });
    state.updateProfile(0, fixed);
    await tester.pumpAndSettle();

    expect(find.text('Valid ✓'), findsOneWidget);
    expect(find.textContaining('error'), findsNothing);
  });

  testWidgets('ValidationBanner still revalidates on profile add/remove',
      (tester) async {
    final schemaService = MockSchemaService();
    final state = ProfilesState();

    when(schemaService.validateProfiles(any))
        .thenAnswer((_) async => ValidationResult.valid());

    await tester.pumpWidget(
      MultiProvider(
        providers: [
          ChangeNotifierProvider<ProfilesState>.value(value: state),
          Provider<SchemaService>.value(value: schemaService),
        ],
        child: const MaterialApp(home: Scaffold(body: ValidationBanner())),
      ),
    );

    state.addProfile(Profile(name: 'P1', buttons: {}));
    await tester.pumpAndSettle();
    state.addProfile(Profile(name: 'P2', buttons: {}));
    await tester.pumpAndSettle();
    state.removeProfile(0);
    await tester.pumpAndSettle();

    // One call per ProfilesState change: 3 mutations.
    // (Plus possibly an initial empty-state call, which we don't assert on.)
    verify(schemaService.validateProfiles(any)).called(greaterThanOrEqualTo(3));
  });
}
