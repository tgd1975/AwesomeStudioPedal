import 'action_config.dart';

class MacroStep {
  MacroStep({required this.actions});

  final List<ActionConfig> actions;

  factory MacroStep.fromJson(List<dynamic> json) {
    return MacroStep(
      actions: json
          .map((e) => ActionConfig.fromJson(e as Map<String, dynamic>))
          .toList(),
    );
  }

  List<Map<String, dynamic>> toJson() =>
      actions.map((a) => a.toJson()).toList();
}
