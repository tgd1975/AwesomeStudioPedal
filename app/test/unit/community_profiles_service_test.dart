import 'dart:io';
import 'package:awesome_studio_pedal/services/community_profiles_service.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:http/http.dart' as http;
import 'package:http/testing.dart';

const _kIndexJson = '''
{
  "version": 1,
  "generated": "2026-01-01T00:00:00.000Z",
  "profiles": [
    {
      "id": "test-profile",
      "file": "2-button/test-profile.json",
      "minButtons": 2,
      "name": "Test Profile",
      "description": "A test profile",
      "author": "tester",
      "tags": ["test"],
      "tested": "macOS 14",
      "profileCount": 1,
      "profiles": ["01 Test"]
    }
  ]
}
''';

const _kProfileJson = '''
{
  "_meta": {"id": "test-profile", "author": "tester", "tags": ["test"]},
  "profiles": [
    {
      "name": "01 Test",
      "buttons": {
        "A": {"type": "SendCharAction", "value": "KEY_PAGE_UP"},
        "B": {"type": "SendCharAction", "value": "KEY_PAGE_DOWN"}
      }
    }
  ]
}
''';

late Directory _tmpDir;

Future<Directory> _tmpCacheDir() async => _tmpDir;

http.Client _mockClient({bool failIndex = false, bool failSet = false}) {
  return MockClient((req) async {
    if (req.url.path.endsWith('index.json')) {
      if (failIndex) return http.Response('error', 500);
      return http.Response(_kIndexJson, 200);
    }
    if (req.url.path.endsWith('test-profile.json')) {
      if (failSet) return http.Response('error', 500);
      return http.Response(_kProfileJson, 200);
    }
    return http.Response('not found', 404);
  });
}

CommunityProfilesService _svc(
        {bool failIndex = false, bool failSet = false, http.Client? client}) =>
    CommunityProfilesService(
      client: client ?? _mockClient(failIndex: failIndex, failSet: failSet),
      cacheDirFactory: _tmpCacheDir,
    );

void main() {
  setUp(() async {
    _tmpDir = await Directory.systemTemp.createTemp('cp_test_');
  });
  tearDown(() async {
    if (await _tmpDir.exists()) await _tmpDir.delete(recursive: true);
  });

  group('CommunityProfilesService', () {
    test('fetchIndex returns parsed entries on fresh fetch', () async {
      final entries = await _svc().fetchIndex();
      expect(entries.length, 1);
      expect(entries[0].id, 'test-profile');
      expect(entries[0].minButtons, 2);
      expect(entries[0].author, 'tester');
    });

    test('fetchIndex returns cached data on second call', () async {
      int calls = 0;
      final client = MockClient((req) async {
        calls++;
        return http.Response(_kIndexJson, 200);
      });
      final svc = _svc(client: client);
      await svc.fetchIndex();
      await svc.fetchIndex();
      expect(calls, 1);
    });

    test('fetchIndex throws on network error', () async {
      expect(
          () => _svc(failIndex: true).fetchIndex(), throwsA(isA<Exception>()));
    });

    test('fetchProfileSet returns parsed json on fresh fetch', () async {
      final json = await _svc().fetchProfileSet('test-profile');
      expect(json['profiles'], isA<List>());
      expect((json['profiles'] as List).length, 1);
    });

    test('fetchProfileSet throws on unknown id', () async {
      expect(
        () => _svc().fetchProfileSet('nonexistent'),
        throwsA(isA<ArgumentError>()),
      );
    });

    test('fetchProfileSet throws on network error', () async {
      expect(
        () => _svc(failSet: true).fetchProfileSet('test-profile'),
        throwsA(isA<Exception>()),
      );
    });

    test('clearCache removes cached files so next fetch goes to network',
        () async {
      int calls = 0;
      final client = MockClient((req) async {
        calls++;
        return http.Response(_kIndexJson, 200);
      });
      final svc = _svc(client: client);
      await svc.fetchIndex();
      await svc.clearCache();
      await svc.fetchIndex();
      expect(calls, 2);
    });
  });
}
