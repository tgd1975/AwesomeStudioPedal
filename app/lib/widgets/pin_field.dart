import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class PinField extends StatelessWidget {
  const PinField({super.key, required this.controller});

  final TextEditingController controller;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          child: TextField(
            controller: controller,
            keyboardType: TextInputType.number,
            inputFormatters: [FilteringTextInputFormatter.digitsOnly],
            decoration: const InputDecoration(
              labelText: 'GPIO Pin (0–39)',
              helperText: 'Enter the ESP32 GPIO pin number',
            ),
          ),
        ),
        const Tooltip(
          message: _kGpioTooltip,
          triggerMode: TooltipTriggerMode.tap,
          child: Padding(
            padding: EdgeInsets.only(left: 8, top: 8),
            child: Icon(Icons.help_outline, size: 20),
          ),
        ),
      ],
    );
  }
}

const String _kGpioTooltip = '''
ESP32 GPIO reference (NodeMCU-32S):
  Buttons:    13, 12, 27, 14
  BT LED:     26
  Power LED:  25
  Select LEDs: 5, 18, 19
  Avoid: 6–11 (flash), 34–39 (input-only)
''';
