import 'dart:convert';
import 'dart:io';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/foundation.dart';
import 'package:path_provider/path_provider.dart';
import 'package:share_plus/share_plus.dart';
import '../models/profiles_state.dart';
import '../services/schema_service.dart';

class ImportResult {
  ImportResult.success()
      : success = true,
        errors = const [];
  ImportResult.failure(this.errors) : success = false;

  final bool success;
  final List<String> errors;
}

class FileService extends ChangeNotifier {
  ProfilesState? profilesState;
  static const _kAutoSaveFile = 'profiles_autosave.json';
  static const _kConfigAutoSaveFile = 'config_autosave.json';

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
    if (!validation.isValid && validation.isSchemaError) {
      return ImportResult.failure(validation.errors);
    }

    // Runtime-resolvability errors do not block import — the user must
    // be able to see and fix the bad values inside the app. The
    // ValidationBanner will turn red so the issue is visible. See
    // TASK-264.
    profilesState?.loadFromJson(json);
    await _autoSave();
    return ImportResult.success();
  }

  Future<ImportResult> importHardwareConfig(SchemaService schema) async {
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

    final validation = await schema.validateConfig(json);
    if (!validation.isValid) {
      return ImportResult.failure(validation.errors);
    }

    profilesState?.loadHardwareConfigFromJson(json);
    await _autoSaveConfig();
    return ImportResult.success();
  }

  Future<void> exportProfiles() async {
    final json = profilesState?.toProfilesJsonString() ?? '{}';
    final filename = exportFilenameForDate(DateTime.now());
    final dir = await getTemporaryDirectory();
    final file = File('${dir.path}/$filename');
    await file.writeAsString(json);
    await Share.shareXFiles(
      [XFile(file.path, mimeType: 'application/json', name: filename)],
      subject: filename,
    );
  }

  /// Date-stamped export filename, e.g. `profiles-2026-04-26.json`.
  /// Public so it can be unit-tested without going through the share sheet.
  static String exportFilenameForDate(DateTime now) {
    final y = now.year.toString().padLeft(4, '0');
    final m = now.month.toString().padLeft(2, '0');
    final d = now.day.toString().padLeft(2, '0');
    return 'profiles-$y-$m-$d.json';
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

  Future<void> _autoSaveConfig() async {
    final state = profilesState;
    if (state == null || state.hardwareConfig == null) return;
    try {
      final dir = await getApplicationDocumentsDirectory();
      await File('${dir.path}/$_kConfigAutoSaveFile')
          .writeAsString(state.toConfigJsonString());
    } catch (e) {
      debugPrint('Config auto-save failed: $e');
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

  Future<void> restoreConfigAutoSave() async {
    try {
      final dir = await getApplicationDocumentsDirectory();
      final file = File('${dir.path}/$_kConfigAutoSaveFile');
      if (!file.existsSync()) return;
      final content = await file.readAsString();
      final json = jsonDecode(content) as Map<String, dynamic>;
      profilesState?.loadHardwareConfigFromJson(json);
    } catch (e) {
      debugPrint('Config auto-save restore failed: $e');
    }
  }

  void notifyAutoSave() {
    _autoSave();
  }
}
