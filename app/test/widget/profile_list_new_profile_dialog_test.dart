import 'package:awesome_studio_pedal/constants/profile_constants.dart';
import 'package:awesome_studio_pedal/models/profile.dart';
import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:awesome_studio_pedal/screens/profile_list_screen.dart';
import 'package:awesome_studio_pedal/services/file_service.dart';
import 'package:awesome_studio_pedal/services/schema_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

Widget _host(Widget child, {ProfilesState? state}) => MaterialApp(
      home: MultiProvider(
        providers: [
          ChangeNotifierProvider(create: (_) => state ?? ProfilesState()),
          Provider(create: (_) => SchemaService()),
          ChangeNotifierProvider(create: (_) => FileService()),
        ],
        child: child,
      ),
    );

ProfilesState _stateWith(int n) {
  final state = ProfilesState();
  for (var i = 0; i < n; i++) {
    state.addProfile(Profile(name: 'P${i + 1}', buttons: const {}));
  }
  return state;
}

void main() {
  group('ProfileListScreen new-profile dialog', () {
    testWidgets('blank Name on Add shows an inline "Name is required" error',
        (tester) async {
      await tester.pumpWidget(_host(const ProfileListScreen()));
      await tester.tap(find.byType(FloatingActionButton));
      await tester.pumpAndSettle();

      expect(find.text('New Profile'), findsOneWidget);
      expect(find.text('Name is required'), findsNothing);

      await tester.tap(find.widgetWithText(FilledButton, 'Add'));
      await tester.pumpAndSettle();

      expect(find.text('Name is required'), findsOneWidget);
      expect(find.text('New Profile'), findsOneWidget);
    });

    testWidgets('valid Name on Add closes the dialog and adds the profile',
        (tester) async {
      await tester.pumpWidget(_host(const ProfileListScreen()));
      await tester.tap(find.byType(FloatingActionButton));
      await tester.pumpAndSettle();

      await tester.enterText(
          find.widgetWithText(TextFormField, 'Name'), 'My Profile');
      await tester.tap(find.widgetWithText(FilledButton, 'Add'));
      await tester.pumpAndSettle();

      expect(find.text('New Profile'), findsNothing);
      expect(find.text('My Profile'), findsOneWidget);
    });
  });

  group('ProfileListScreen Add-profile cap (TASK-279)', () {
    testWidgets('FAB is enabled below the cap', (tester) async {
      final state = _stateWith(kMaxProfiles - 1);
      await tester.pumpWidget(_host(const ProfileListScreen(), state: state));
      await tester.pumpAndSettle();

      final fab = tester
          .widget<FloatingActionButton>(find.byType(FloatingActionButton));
      expect(fab.onPressed, isNotNull);
      expect(fab.tooltip, equals('Add profile'));
    });

    testWidgets('FAB is disabled at the cap and tooltip names the limit',
        (tester) async {
      final state = _stateWith(kMaxProfiles);
      await tester.pumpWidget(_host(const ProfileListScreen(), state: state));
      await tester.pumpAndSettle();

      final fab = tester
          .widget<FloatingActionButton>(find.byType(FloatingActionButton));
      expect(fab.onPressed, isNull);
      expect(fab.tooltip, contains('$kMaxProfiles'));
    });

    testWidgets('FAB flips back to enabled after a profile is removed',
        (tester) async {
      final state = _stateWith(kMaxProfiles);
      await tester.pumpWidget(_host(const ProfileListScreen(), state: state));
      await tester.pumpAndSettle();

      // At cap: disabled.
      var fab = tester
          .widget<FloatingActionButton>(find.byType(FloatingActionButton));
      expect(fab.onPressed, isNull);

      // Remove one → now under cap.
      state.removeProfile(0);
      await tester.pumpAndSettle();

      fab = tester
          .widget<FloatingActionButton>(find.byType(FloatingActionButton));
      expect(fab.onPressed, isNotNull);
    });
  });
}
