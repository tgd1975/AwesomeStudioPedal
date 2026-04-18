import 'action_config.dart';

class Profile {
  Profile({
    required this.name,
    this.description,
    required this.buttons,
  });

  final String name;
  final String? description;
  final Map<String, ActionConfig> buttons;

  factory Profile.fromJson(Map<String, dynamic> json) {
    final buttons = (json['buttons'] as Map<String, dynamic>).map(
      (k, v) => MapEntry(k, ActionConfig.fromJson(v as Map<String, dynamic>)),
    );
    return Profile(
      name: json['name'] as String,
      description: json['description'] as String?,
      buttons: buttons,
    );
  }

  Profile copyWith({
    String? name,
    String? description,
    Map<String, ActionConfig>? buttons,
  }) {
    return Profile(
      name: name ?? this.name,
      description: description ?? this.description,
      buttons: buttons ?? this.buttons,
    );
  }

  Map<String, dynamic> toJson() {
    final m = <String, dynamic>{
      'name': name,
      'buttons': buttons.map((k, v) => MapEntry(k, v.toJson())),
    };
    if (description != null) m['description'] = description;
    return m;
  }
}
