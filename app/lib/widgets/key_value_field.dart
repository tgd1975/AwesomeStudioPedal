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
      return DropdownButtonFormField<String>(
        value: kMediaKeyValues.contains(controller.text)
            ? controller.text
            : kMediaKeyValues.first,
        decoration: const InputDecoration(labelText: 'Media Key'),
        items: kMediaKeyValues
            .map((v) => DropdownMenuItem(value: v, child: Text(v)))
            .toList(),
        onChanged: (v) {
          if (v != null) controller.text = v;
        },
      );
    }

    if (type == 'SendKeyAction' || type == 'SendCharAction') {
      return Autocomplete<String>(
        initialValue: TextEditingValue(text: controller.text),
        optionsBuilder: (value) {
          if (value.text.isEmpty) return const Iterable.empty();
          final q = value.text.toUpperCase();
          return kKeyNames.where((k) => k.toUpperCase().contains(q));
        },
        onSelected: (v) => controller.text = v,
        fieldViewBuilder: (ctx, ctrl, focus, onSubmit) {
          ctrl.text = controller.text;
          return TextField(
            controller: ctrl,
            focusNode: focus,
            onSubmitted: (_) => onSubmit(),
            decoration: const InputDecoration(labelText: 'Key name'),
          );
        },
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
