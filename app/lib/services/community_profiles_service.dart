import 'dart:convert';
import 'dart:io';
import 'package:http/http.dart' as http;
import 'package:path_provider/path_provider.dart';

const _kBaseUrl = 'https://tgd1975.github.io/AwesomeStudioPedal/profiles';

class ProfileIndexEntry {
  ProfileIndexEntry({
    required this.id,
    required this.file,
    required this.minButtons,
    required this.name,
    required this.description,
    required this.author,
    required this.tags,
    required this.tested,
    required this.profileCount,
    required this.profiles,
    this.minFirmware,
  });

  final String id;
  final String file;
  final int minButtons;
  final String name;
  final String description;
  final String author;
  final List<String> tags;
  final String tested;
  final int profileCount;
  final List<String> profiles;
  final String? minFirmware;

  factory ProfileIndexEntry.fromJson(Map<String, dynamic> json) =>
      ProfileIndexEntry(
        id: json['id'] as String,
        file: json['file'] as String,
        minButtons: json['minButtons'] as int,
        name: json['name'] as String,
        description: json['description'] as String? ?? '',
        author: json['author'] as String,
        tags: List<String>.from(json['tags'] as List),
        tested: json['tested'] as String? ?? '',
        profileCount: json['profileCount'] as int,
        profiles: List<String>.from(json['profiles'] as List),
        minFirmware: json['minFirmware'] as String?,
      );
}

class CommunityProfilesService {
  CommunityProfilesService({
    http.Client? client,
    Future<Directory> Function()? cacheDirFactory,
  })  : _client = client ?? http.Client(),
        _cacheDirFactory = cacheDirFactory;

  final http.Client _client;
  final Future<Directory> Function()? _cacheDirFactory;

  Future<List<ProfileIndexEntry>> fetchIndex() async {
    final cached = await _readCache('index.json');
    if (cached != null) {
      return _parseIndex(cached);
    }

    final res = await _client.get(Uri.parse('$_kBaseUrl/index.json'));
    if (res.statusCode != 200) {
      throw HttpException('HTTP ${res.statusCode}',
          uri: Uri.parse('$_kBaseUrl/index.json'));
    }

    await _writeCache('index.json', res.body);
    return _parseIndex(res.body);
  }

  Future<Map<String, dynamic>> fetchProfileSet(String id) async {
    final index = await fetchIndex();
    final entry = index.firstWhere(
      (e) => e.id == id,
      orElse: () => throw ArgumentError('Unknown profile id: $id'),
    );

    final cacheKey = entry.file.replaceAll('/', '_');
    final cached = await _readCache(cacheKey);
    if (cached != null) {
      return jsonDecode(cached) as Map<String, dynamic>;
    }

    final url = '$_kBaseUrl/${entry.file}';
    final res = await _client.get(Uri.parse(url));
    if (res.statusCode != 200) {
      throw HttpException('HTTP ${res.statusCode}', uri: Uri.parse(url));
    }

    await _writeCache(cacheKey, res.body);
    return jsonDecode(res.body) as Map<String, dynamic>;
  }

  Future<void> clearCache() async {
    final dir = await _cacheDir();
    if (await dir.exists()) {
      await for (final entity in dir.list()) {
        if (entity is File) await entity.delete();
      }
    }
  }

  // ── Private ─────────────────────────────────────────────────────────────────

  List<ProfileIndexEntry> _parseIndex(String body) {
    final data = jsonDecode(body) as Map<String, dynamic>;
    return (data['profiles'] as List)
        .map((e) => ProfileIndexEntry.fromJson(e as Map<String, dynamic>))
        .toList();
  }

  Future<Directory> _cacheDir() async {
    if (_cacheDirFactory != null) return _cacheDirFactory!();
    final base = await getApplicationCacheDirectory();
    final dir = Directory('${base.path}/community_profiles');
    if (!await dir.exists()) await dir.create(recursive: true);
    return dir;
  }

  Future<String?> _readCache(String key) async {
    try {
      final file = File('${(await _cacheDir()).path}/$key');
      if (await file.exists()) return file.readAsString();
    } catch (_) {}
    return null;
  }

  Future<void> _writeCache(String key, String body) async {
    try {
      final file = File('${(await _cacheDir()).path}/$key');
      await file.writeAsString(body);
    } catch (_) {}
  }
}
