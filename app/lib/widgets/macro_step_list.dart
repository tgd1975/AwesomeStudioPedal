import 'package:flutter/material.dart';
import '../models/action_config.dart';
import '../models/macro_step.dart';
import '../screens/action_editor_screen.dart';

class MacroStepList extends StatefulWidget {
  const MacroStepList({
    super.key,
    this.initial,
    required this.onChanged,
  });

  final List<MacroStep>? initial;
  final void Function(List<MacroStep>) onChanged;

  @override
  State<MacroStepList> createState() => _MacroStepListState();
}

class _MacroStepListState extends State<MacroStepList> {
  late List<MacroStep> _steps;

  @override
  void initState() {
    super.initState();
    _steps = widget.initial != null
        ? widget.initial!.map((s) => MacroStep(actions: List.from(s.actions))).toList()
        : [];
  }

  void _notify() => widget.onChanged(List.from(_steps));

  void _addStep() {
    setState(() => _steps.add(MacroStep(actions: [])));
    _notify();
  }

  void _removeStep(int i) {
    setState(() => _steps.removeAt(i));
    _notify();
  }

  void _addAction(int stepIndex) {
    // Opens an embedded ActionEditorScreen overlay via dialog.
    ActionConfig? pending;
    showDialog<void>(
      context: context,
      builder: (ctx) => AlertDialog(
        title: Text('Add action to step ${stepIndex + 1}'),
        content: ActionEditorScreen(
          buttonId: 'step-$stepIndex',
          onSave: (a) {
            pending = a;
            Navigator.of(ctx).pop();
          },
          embeddedMode: true,
        ),
        actions: const [],
      ),
    ).then((_) {
      if (pending != null) {
        setState(() => _steps[stepIndex].actions.add(pending!));
        _notify();
      }
    });
  }

  void _removeAction(int stepIndex, int actionIndex) {
    setState(() => _steps[stepIndex].actions.removeAt(actionIndex));
    _notify();
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        for (int si = 0; si < _steps.length; si++) ...[
          Card(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                ListTile(
                  title: Text('Step ${si + 1}'),
                  trailing: IconButton(
                    icon: const Icon(Icons.delete_outline),
                    onPressed: () => _removeStep(si),
                  ),
                ),
                for (int ai = 0; ai < _steps[si].actions.length; ai++)
                  ListTile(
                    contentPadding: const EdgeInsets.only(left: 32, right: 8),
                    title: Text(_steps[si].actions[ai].type),
                    subtitle: _steps[si].actions[ai].name != null
                        ? Text(_steps[si].actions[ai].name!)
                        : null,
                    trailing: IconButton(
                      icon: const Icon(Icons.close, size: 18),
                      onPressed: () => _removeAction(si, ai),
                    ),
                  ),
                Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 4),
                  child: TextButton.icon(
                    icon: const Icon(Icons.add, size: 16),
                    label: const Text('Add action'),
                    onPressed: () => _addAction(si),
                  ),
                ),
              ],
            ),
          ),
        ],
        TextButton.icon(
          icon: const Icon(Icons.add),
          label: const Text('Add step'),
          onPressed: _addStep,
        ),
      ],
    );
  }
}
