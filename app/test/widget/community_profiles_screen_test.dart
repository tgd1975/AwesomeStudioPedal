import 'package:awesome_studio_pedal/models/profiles_state.dart';
import 'package:awesome_studio_pedal/screens/community_profiles_screen.dart';
import 'package:awesome_studio_pedal/services/community_profiles_service.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';

final _kEntries = [
  ProfileIndexEntry(
    id: 'page-turner',
    file: '2-button/page-turner-basic.json',
    minButtons: 2,
    name: 'Page Turner',
    description: 'Turn pages in sheet music apps.',
    author: 'tester',
    tags: ['sheet-music', 'musicians'],
    tested: 'macOS 14',
    profileCount: 1,
    profiles: ['01 Page Turner'],
  ),
  ProfileIndexEntry(
    id: 'obs-deck',
    file: '4-button/obs-stream-deck.json',
    minButtons: 4,
    name: 'OBS Stream Deck',
    description: 'Switch OBS scenes.',
    author: 'tester',
    tags: ['obs', 'streaming'],
    tested: 'Windows 11',
    profileCount: 1,
    profiles: ['01 Scene Switcher'],
  ),
];

class _FakeService extends CommunityProfilesService {
  _FakeService({this.failIndex = false}) : super();

  final bool failIndex;

  @override
  Future<List<ProfileIndexEntry>> fetchIndex() async {
    if (failIndex) throw Exception('network error');
    return _kEntries;
  }

  @override
  Future<Map<String, dynamic>> fetchProfileSet(String id) async {
    return {
      'profiles': [
        {
          'name': '01 Page Turner',
          'buttons': {
            'A': {'type': 'SendCharAction', 'value': 'KEY_PAGE_UP'},
            'B': {'type': 'SendCharAction', 'value': 'KEY_PAGE_DOWN'},
          }
        }
      ]
    };
  }
}

Widget _wrap(Widget child, {ProfilesState? state}) {
  return ChangeNotifierProvider<ProfilesState>.value(
    value: state ?? ProfilesState(),
    child: MaterialApp.router(
      routerConfig: GoRouter(
        initialLocation: '/',
        routes: [
          GoRoute(path: '/', builder: (_, __) => child),
          GoRoute(
            path: '/profiles',
            builder: (_, __) => const Scaffold(body: Text('ProfileList')),
          ),
        ],
      ),
    ),
  );
}

void main() {
  group('CommunityProfilesScreen', () {
    testWidgets('renders card list after loading', (tester) async {
      await tester
          .pumpWidget(_wrap(CommunityProfilesScreen(service: _FakeService())));
      await tester.pump();
      await tester.pump();

      expect(find.text('Page Turner'), findsOneWidget);
      expect(find.text('OBS Stream Deck'), findsOneWidget);
    });

    testWidgets('search filter hides non-matching cards', (tester) async {
      await tester
          .pumpWidget(_wrap(CommunityProfilesScreen(service: _FakeService())));
      await tester.pump();
      await tester.pump();

      await tester.enterText(find.byType(TextField), 'obs');
      await tester.pump();

      expect(find.text('OBS Stream Deck'), findsOneWidget);
      expect(find.text('Page Turner'), findsNothing);
    });

    testWidgets('button-count filter hides incompatible cards', (tester) async {
      await tester
          .pumpWidget(_wrap(CommunityProfilesScreen(service: _FakeService())));
      await tester.pump();
      await tester.pump();

      await tester.tap(find.byType(DropdownButton<int?>));
      await tester.pumpAndSettle();
      await tester.tap(find.text('2-btn').last);
      await tester.pump();

      expect(find.text('Page Turner'), findsOneWidget);
      expect(find.text('OBS Stream Deck'), findsNothing);
    });

    testWidgets('shows error state with retry on network failure',
        (tester) async {
      await tester.pumpWidget(_wrap(
          CommunityProfilesScreen(service: _FakeService(failIndex: true))));
      await tester.pump();
      await tester.pump();

      expect(find.text('Retry'), findsOneWidget);
    });
  });
}
