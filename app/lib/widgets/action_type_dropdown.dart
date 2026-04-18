import 'package:flutter/material.dart';
import '../constants/action_types.dart';

class ActionTypeDropdown extends StatelessWidget {
  const ActionTypeDropdown({
    super.key,
    required this.value,
    required this.onChanged,
  });

  final String value;
  final void Function(String) onChanged;

  @override
  Widget build(BuildContext context) {
    return DropdownButtonFormField<String>(
      value: value,
      decoration: const InputDecoration(labelText: 'Action Type'),
      items: kActionTypes
          .map((opt) => DropdownMenuItem<String>(
                value: opt.value,
                child: Text(opt.label),
              ))
          .toList(),
      onChanged: (v) {
        if (v != null) onChanged(v);
      },
    );
  }
}
