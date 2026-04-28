import 'macro_step.dart';

class ActionConfig {
  ActionConfig({
    required this.type,
    this.value,
    this.pin,
    this.delayMs,
    this.name,
    this.steps,
    this.action,
    this.longPress,
    this.doublePress,
  });

  final String type;
  final String? value;
  final int? pin;
  final int? delayMs;
  final String? name;
  // For MacroAction: list of steps, each step is a list of actions.
  final List<MacroStep>? steps;
  // For DelayedAction: the nested action.
  final ActionConfig? action;
  // Optional long-press / double-press sub-actions.
  final ActionConfig? longPress;
  final ActionConfig? doublePress;

  factory ActionConfig.fromJson(Map<String, dynamic> json) {
    return ActionConfig(
      type: json['type'] as String,
      value: json['value'] as String?,
      pin: (json['pin'] as num?)?.toInt(),
      delayMs: (json['delayMs'] as num?)?.toInt(),
      name: json['name'] as String?,
      action: json['action'] != null
          ? ActionConfig.fromJson(json['action'] as Map<String, dynamic>)
          : null,
      steps: json['steps'] != null
          ? (json['steps'] as List<dynamic>)
              .map((s) => MacroStep.fromJson(s as List<dynamic>))
              .toList()
          : null,
      longPress: json['longPress'] != null
          ? ActionConfig.fromJson(json['longPress'] as Map<String, dynamic>)
          : null,
      doublePress: json['doublePress'] != null
          ? ActionConfig.fromJson(json['doublePress'] as Map<String, dynamic>)
          : null,
    );
  }

  Map<String, dynamic> toJson() {
    final m = <String, dynamic>{'type': type};
    if (value != null) m['value'] = value;
    if (pin != null) m['pin'] = pin;
    if (delayMs != null) m['delayMs'] = delayMs;
    if (name != null) m['name'] = name;
    if (action != null) m['action'] = action!.toJson();
    if (steps != null) m['steps'] = steps!.map((s) => s.toJson()).toList();
    if (longPress != null) m['longPress'] = longPress!.toJson();
    if (doublePress != null) m['doublePress'] = doublePress!.toJson();
    return m;
  }
}
