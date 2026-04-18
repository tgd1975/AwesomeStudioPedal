import 'dart:convert';
import 'package:flutter/foundation.dart';
import 'profile.dart';
import 'hardware_config.dart';

class ProfilesState extends ChangeNotifier {
  List<Profile> _profiles = [];
  HardwareConfig? _hardwareConfig;
  DateTime? _lastModified;

  List<Profile> get profiles => List.unmodifiable(_profiles);
  HardwareConfig? get hardwareConfig => _hardwareConfig;
  DateTime? get lastModified => _lastModified;

  void loadFromJson(Map<String, dynamic> json) {
    _profiles = (json['profiles'] as List<dynamic>)
        .map((p) => Profile.fromJson(p as Map<String, dynamic>))
        .toList();
    _lastModified = DateTime.now();
    notifyListeners();
  }

  void loadHardwareConfigFromJson(Map<String, dynamic> json) {
    _hardwareConfig = HardwareConfig.fromJson(json);
    notifyListeners();
  }

  void addProfile(Profile profile) {
    _profiles.add(profile);
    _lastModified = DateTime.now();
    notifyListeners();
  }

  void removeProfile(int index) {
    _profiles.removeAt(index);
    _lastModified = DateTime.now();
    notifyListeners();
  }

  void reorderProfiles(int oldIndex, int newIndex) {
    final item = _profiles.removeAt(oldIndex);
    _profiles.insert(newIndex, item);
    _lastModified = DateTime.now();
    notifyListeners();
  }

  void updateProfile(int index, Profile profile) {
    _profiles[index] = profile;
    _lastModified = DateTime.now();
    notifyListeners();
  }

  Map<String, dynamic> toProfilesJson() => {
        'profiles': _profiles.map((p) => p.toJson()).toList(),
      };

  String toProfilesJsonString() =>
      const JsonEncoder.withIndent('  ').convert(toProfilesJson());

  String toConfigJsonString() {
    if (_hardwareConfig == null) return '{}';
    return const JsonEncoder.withIndent('  ')
        .convert(_hardwareConfig!.toJson());
  }
}
