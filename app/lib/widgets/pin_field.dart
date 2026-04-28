import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import '../models/hardware_config.dart';

class PinField extends StatelessWidget {
  const PinField({
    super.key,
    required this.controller,
    this.board = BoardTarget.esp32,
  });

  final TextEditingController controller;
  final BoardTarget board;

  @override
  Widget build(BuildContext context) {
    final info = _kBoardPinInfo[board]!;
    return Row(
      children: [
        Expanded(
          child: TextField(
            controller: controller,
            keyboardType: TextInputType.number,
            inputFormatters: [FilteringTextInputFormatter.digitsOnly],
            decoration: InputDecoration(
              labelText: 'GPIO Pin (0–${info.maxPin})',
              helperText: 'Enter the ${info.chipLabel} GPIO pin number',
            ),
          ),
        ),
        Tooltip(
          message: info.tooltip,
          triggerMode: TooltipTriggerMode.tap,
          child: const Padding(
            padding: EdgeInsets.only(left: 8, top: 8),
            child: Icon(Icons.help_outline, size: 20),
          ),
        ),
      ],
    );
  }
}

class _PinInfo {
  const _PinInfo({
    required this.chipLabel,
    required this.maxPin,
    required this.tooltip,
  });
  final String chipLabel;
  final int maxPin;
  final String tooltip;
}

const _kBoardPinInfo = <BoardTarget, _PinInfo>{
  BoardTarget.esp32: _PinInfo(
    chipLabel: 'ESP32',
    maxPin: 39,
    tooltip: '''
ESP32 GPIO reference (NodeMCU-32S):
  Buttons:     13, 12, 27, 14
  BT LED:      26
  Power LED:   25
  Select LEDs: 5, 18, 19
  Avoid: 6-11 (flash), 34-39 (input-only)
''',
  ),
  BoardTarget.nrf52840: _PinInfo(
    chipLabel: 'nRF52840',
    maxPin: 47,
    tooltip: '''
nRF52840 GPIO reference:
  Pins: P0.00-P0.31, P1.00-P1.15
  Encoded as: P0.xx = xx, P1.xx = 32+xx
  Avoid: 9, 10 (NFC), 17-20 (SWD/UART)
''',
  ),
};
