import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import '../models/action_config.dart';
import '../widgets/action_type_dropdown.dart';
import '../widgets/key_value_field.dart';
import '../widgets/pin_field.dart';
import '../constants/action_types.dart';

class ActionEditorScreen extends StatefulWidget {
  const ActionEditorScreen({
    super.key,
    required this.buttonId,
    this.initial,
    required this.onSave,
    this.embeddedMode = false,
  });

  final String buttonId;
  final ActionConfig? initial;
  final void Function(ActionConfig) onSave;
  /// When true, renders as an inline widget rather than a full Scaffold screen.
  final bool embeddedMode;

  @override
  State<ActionEditorScreen> createState() => _ActionEditorScreenState();
}

class _ActionEditorScreenState extends State<ActionEditorScreen> {
  late String _type;
  late TextEditingController _valueCtrl;
  late TextEditingController _nameCtrl;
  late TextEditingController _pinCtrl;

  @override
  void initState() {
    super.initState();
    _type = widget.initial?.type ?? '';
    _valueCtrl = TextEditingController(text: widget.initial?.value ?? '');
    _nameCtrl = TextEditingController(text: widget.initial?.name ?? '');
    _pinCtrl = TextEditingController(
      text: widget.initial?.pin?.toString() ?? '',
    );
  }

  @override
  void dispose() {
    _valueCtrl.dispose();
    _nameCtrl.dispose();
    _pinCtrl.dispose();
    super.dispose();
  }

  bool get _isPin => kPinTypes.contains(_type);
  bool get _needsValue =>
      _type == 'SendCharAction' ||
      _type == 'SendKeyAction' ||
      _type == 'SendStringAction' ||
      _type == 'SendMediaKeyAction' ||
      _type == 'SerialOutputAction';

  void _save() {
    final config = ActionConfig(
      type: _type,
      value: _needsValue ? _valueCtrl.text : null,
      pin: _isPin ? int.tryParse(_pinCtrl.text) : null,
      name: _nameCtrl.text.isEmpty ? null : _nameCtrl.text,
    );
    widget.onSave(config);
    context.pop();
  }

  Widget _buildFields() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        ActionTypeDropdown(
          value: _type,
          onChanged: (v) => setState(() => _type = v),
        ),
        const SizedBox(height: 16),
        if (_needsValue)
          KeyValueField(type: _type, controller: _valueCtrl),
        if (_isPin)
          PinField(controller: _pinCtrl),
        const SizedBox(height: 16),
        TextField(
          controller: _nameCtrl,
          decoration: const InputDecoration(
            labelText: 'Display name (optional)',
            hintText: 'e.g. Play/Pause',
          ),
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    if (widget.embeddedMode) {
      return Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          _buildFields(),
          const SizedBox(height: 8),
          Align(
            alignment: Alignment.centerRight,
            child: FilledButton(
              onPressed: _type.isNotEmpty ? _save : null,
              child: const Text('Apply'),
            ),
          ),
        ],
      );
    }
    return Scaffold(
      appBar: AppBar(
        title: Text('Button ${widget.buttonId}'),
        actions: [
          TextButton(
            onPressed: _type.isNotEmpty ? _save : null,
            child: const Text('Save'),
          ),
        ],
      ),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [_buildFields()],
      ),
    );
  }
}
