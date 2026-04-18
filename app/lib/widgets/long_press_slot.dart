import 'package:flutter/material.dart';
import '../models/action_config.dart';
import '../screens/action_editor_screen.dart';

class LongPressSlot extends StatefulWidget {
  const LongPressSlot({
    super.key,
    this.initial,
    required this.onChanged,
  });

  final ActionConfig? initial;
  final void Function(ActionConfig?) onChanged;

  @override
  State<LongPressSlot> createState() => _LongPressSlotState();
}

class _LongPressSlotState extends State<LongPressSlot> {
  bool _enabled = false;
  bool _expanded = false;

  @override
  void initState() {
    super.initState();
    _enabled = widget.initial != null;
    _expanded = _enabled;
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Column(
        children: [
          SwitchListTile(
            title: const Text('Long press action'),
            value: _enabled,
            onChanged: (v) {
              setState(() {
                _enabled = v;
                _expanded = v;
              });
              if (!v) widget.onChanged(null);
            },
          ),
          if (_enabled)
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              child: ActionEditorScreen(
                buttonId: 'longPress',
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
