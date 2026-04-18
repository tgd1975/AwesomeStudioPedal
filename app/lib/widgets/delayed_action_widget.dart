import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import '../models/action_config.dart';
import '../screens/action_editor_screen.dart';

class DelayedActionWidget extends StatefulWidget {
  const DelayedActionWidget({
    super.key,
    this.initial,
    required this.onChanged,
  });

  final ActionConfig? initial;
  final void Function(ActionConfig) onChanged;

  @override
  State<DelayedActionWidget> createState() => _DelayedActionWidgetState();
}

class _DelayedActionWidgetState extends State<DelayedActionWidget> {
  late TextEditingController _delayCtrl;
  ActionConfig? _nestedAction;
  bool _expanded = true;

  @override
  void initState() {
    super.initState();
    _delayCtrl = TextEditingController(
      text: widget.initial?.delayMs?.toString() ?? '0',
    );
    _nestedAction = widget.initial?.action;
  }

  @override
  void dispose() {
    _delayCtrl.dispose();
    super.dispose();
  }

  void _notify() {
    if (_nestedAction == null) return;
    widget.onChanged(ActionConfig(
      type: 'DelayedAction',
      delayMs: int.tryParse(_delayCtrl.text) ?? 0,
      action: _nestedAction,
    ));
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      child: ExpansionTile(
        initiallyExpanded: _expanded,
        onExpansionChanged: (v) => setState(() => _expanded = v),
        title: const Text('Delayed Action'),
        children: [
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
            child: TextField(
              controller: _delayCtrl,
              keyboardType: TextInputType.number,
              inputFormatters: [FilteringTextInputFormatter.digitsOnly],
              decoration: const InputDecoration(labelText: 'Delay (ms)'),
              onChanged: (_) => _notify(),
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text('Action after delay',
                    style: Theme.of(context).textTheme.labelMedium),
                const SizedBox(height: 8),
                ActionEditorScreen(
                  buttonId: 'delayed',
                  initial: _nestedAction,
                  onSave: (a) {
                    setState(() => _nestedAction = a);
                    _notify();
                  },
                  embeddedMode: true,
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
