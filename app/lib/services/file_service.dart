import 'dart:convert';
import 'dart:io';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/foundation.dart';
import 'package:path_provider/path_provider.dart';
import 'package:share_plus/share_plus.dart';
import '../models/profiles_state.dart';
import '../services/schema_service.dart';

class ImportResult {
  ImportResult.success() : success = true, errors = const [];
  ImportResult.failure(this.errors) : success = false;

  final bool success;
  final List<String> errors;
}

class FileService extends ChangeNotifier {
  ProfilesState? profilesState;
  static const _kAutoSaveFile = 'profiles_autosave.json';

  Future<ImportResult> importProfiles(SchemaService schema) async {
    final result = await FilePicker.platform.pickFiles(
      type: FileType.custom,
      allowedExtensions: ['json'],
    );
    if (result == null || result.files.isEmpty) {
      return ImportResult.failure(['No file selected']);
    }

    final path = result.files.single.path;
    if (path == null) return ImportResult.failure(['Cannot read file path']);

    final content = await File(path).readAsString();
    final Map<String, dynamic> json;
    try {
      json = jsonDecode(content) as Map<String, dynamic>;
    } on FormatException catch (e) {
      return ImportResult.failure(['JSON parse error: ${e.message}']);
    }

    final validation = await schema.validateProfiles(json);
    if (!validation.isValid) {
      return ImportResult.failure(validation.errors);
    }

    profilesState?.loadFromJson(json);
    await _autoSave();
    return ImportResult.success();
  }

  Future<void> exportProfiles() async {
    final json = profilesState?.toProfilesJsonString() ?? '{}';
    await Share.shareXFiles(
      [
        XFile.fromData(
          utf8.encode(json),
          name: 'profiles.json',
          mimeType: 'application/json',
        ),
      ],
      subject: 'profiles.json',
    );
  }

  Future<void> saveToDocuments() async {
    final json = profilesState?.toProfilesJsonString() ?? '{}';
    final dir = await getApplicationDocumentsDirectory();
    final file = File('${dir.path}/profiles.json');
    await file.writeAsString(json);
  }

  Future<void> _autoSave() async {
    final json = profilesState?.toProfilesJsonString();
    if (json == null) return;
    try {
      final dir = await getApplicationDocumentsDirectory();
      await File('${dir.path}/$_kAutoSaveFile').writeAsString(json);
    } catch (e) {
      debugPrint('Auto-save failed: $e');
    }
  }

  Future<void> restoreAutoSave() async {
    try {
      final dir = await getApplicationDocumentsDirectory();
      final file = File('${dir.path}/$_kAutoSaveFile');
      if (!file.existsSync()) return;
      final content = await file.readAsString();
      final json = jsonDecode(content) as Map<String, dynamic>;
      profilesState?.loadFromJson(json);
    } catch (e) {
      debugPrint('Auto-save restore failed: $e');
    }
  }

  void notifyAutoSave() {
    _autoSave();
  }
}
