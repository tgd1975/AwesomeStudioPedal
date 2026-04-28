import 'package:flutter/material.dart';
import '../constants/action_types.dart';

class KeyValueField extends StatelessWidget {
  const KeyValueField({
    super.key,
    required this.type,
    required this.controller,
  });

  final String type;
  final TextEditingController controller;

  @override
  Widget build(BuildContext context) {
    if (type == 'SendMediaKeyAction') {
      return _NameAutocomplete(
        label: 'Media Key',
        options: kMediaKeyValues,
        controller: controller,
      );
    }

    if (type == kKeyNamedSentinel || type == 'SendCharAction') {
      return _NameAutocomplete(
        label: 'Key name',
        options: kKeyNames,
        controller: controller,
      );
    }

    if (type == 'SendKeyAction') {
      // Firmware-internal HID code, NOT the USB HID Usage ID. Enter is
      // 0xB0 (not 0x28); see idea-039 / TASK-277. Picking 0xB0 as the
      // example here means a user who copies it directly gets Enter on
      // the host instead of nothing.
      return TextField(
        controller: controller,
        decoration: const InputDecoration(
          labelText: 'Firmware HID code (e.g. 0xB0 = Enter)',
          helperText:
              'Firmware-internal codes, not USB HID Usage IDs (e.g. 0x28 ≠ Enter here).',
        ),
      );
    }

    return TextField(
      controller: controller,
      decoration: InputDecoration(
        labelText: type == 'SendStringAction'
            ? 'String to type'
            : type == 'SerialOutputAction'
                ? 'Serial output string'
                : 'Value',
      ),
    );
  }
}

class _NameAutocomplete extends StatelessWidget {
  const _NameAutocomplete({
    required this.label,
    required this.options,
    required this.controller,
  });

  final String label;
  final List<String> options;
  final TextEditingController controller;

  @override
  Widget build(BuildContext context) {
    return Autocomplete<String>(
      initialValue: TextEditingValue(text: controller.text),
      optionsBuilder: (value) {
        if (value.text.isEmpty) return const Iterable.empty();
        final q = value.text.toUpperCase();
        return options.where((k) => k.toUpperCase().contains(q));
      },
      onSelected: (v) => controller.text = v,
      fieldViewBuilder: (ctx, ctrl, focus, onSubmit) {
        ctrl.text = controller.text;
        return TextField(
          controller: ctrl,
          focusNode: focus,
          onSubmitted: (_) => onSubmit(),
          decoration: InputDecoration(labelText: label),
        );
      },
    );
  }
}
