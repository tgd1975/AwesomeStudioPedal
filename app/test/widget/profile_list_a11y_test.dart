import 'package:awesome_studio_pedal/models/profile.dart';
import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:awesome_studio_pedal/screens/profile_list_screen.dart';
import 'package:awesome_studio_pedal/services/file_service.dart';
import 'package:awesome_studio_pedal/services/schema_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:provider/provider.dart';

Widget _host(ProfilesState state) => MaterialApp(
      home: MultiProvider(
        providers: [
          ChangeNotifierProvider<ProfilesState>.value(value: state),
          Provider(create: (_) => SchemaService()),
          ChangeNotifierProvider(create: (_) => FileService()),
        ],
        child: const ProfileListScreen(),
      ),
    );

void main() {
  group('ProfileListScreen accessibility', () {
    testWidgets('FAB exposes "Add profile" via tooltip / Semantics',
        (tester) async {
      await tester.pumpWidget(_host(ProfilesState()));
      await tester.pumpAndSettle();

      final fab = tester.widget<FloatingActionButton>(
        find.byType(FloatingActionButton),
      );
      expect(fab.tooltip, 'Add profile');
    });

    testWidgets(
        'Trash button on each row exposes "Delete profile <name>" tooltip',
        (tester) async {
      final state = ProfilesState();
      state.addProfile(Profile(name: 'My Profile', buttons: const {}));
      await tester.pumpWidget(_host(state));
      await tester.pumpAndSettle();

      final trash = tester
          .widgetList<IconButton>(find.byType(IconButton))
          .firstWhere((b) =>
              b.icon is Icon && (b.icon as Icon).icon == Icons.delete_outline);
      expect(trash.tooltip, 'Delete profile My Profile');
    });

    testWidgets(
        'Reorder handle on each row exposes "Reorder profile <name>" Semantics',
        (tester) async {
      final handle = tester.ensureSemantics();
      final state = ProfilesState();
      state.addProfile(Profile(name: 'My Profile', buttons: const {}));
      await tester.pumpWidget(_host(state));
      await tester.pumpAndSettle();

      expect(
        find.bySemanticsLabel(RegExp('Reorder profile My Profile')),
        findsWidgets,
      );
      handle.dispose();
    });
  });
}
