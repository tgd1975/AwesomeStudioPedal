import 'package:flutter/material.dart';
import '../models/action_config.dart';
import '../screens/action_editor_screen.dart';

class DoublePressSlot extends StatefulWidget {
  const DoublePressSlot({
    super.key,
    this.initial,
    required this.onChanged,
  });

  final ActionConfig? initial;
  final void Function(ActionConfig?) onChanged;

  @override
  State<DoublePressSlot> createState() => _DoublePressSlotState();
}

class _DoublePressSlotState extends State<DoublePressSlot> {
  bool _enabled = false;

  @override
  void initState() {
    super.initState();
    _enabled = widget.initial != null;
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Column(
        children: [
          SwitchListTile(
            title: const Text('Double press action'),
            value: _enabled,
            onChanged: (v) {
              setState(() => _enabled = v);
              if (!v) widget.onChanged(null);
            },
          ),
          if (_enabled)
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              child: ActionEditorScreen(
                buttonId: 'doublePress',
                initial: widget.initial,
                onSave: widget.onChanged,
                embeddedMode: true,
              ),
            ),
        ],
      ),
    );
  }
}
