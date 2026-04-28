import 'package:awesome_studio_pedal/models/hardware_config.dart';
import 'package:awesome_studio_pedal/widgets/pin_field.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

Widget _wrap(Widget child) => MaterialApp(home: Scaffold(body: child));

void main() {
  group('PinField', () {
    testWidgets('shows ESP32 label and range by default', (tester) async {
      final ctrl = TextEditingController();
      await tester.pumpWidget(_wrap(PinField(controller: ctrl)));

      expect(find.text('GPIO Pin (0–39)'), findsOneWidget);
      expect(find.text('Enter the ESP32 GPIO pin number'), findsOneWidget);
    });

    testWidgets('shows nRF52840 label and range when board is nrf52840',
        (tester) async {
      final ctrl = TextEditingController();
      await tester.pumpWidget(
        _wrap(PinField(controller: ctrl, board: BoardTarget.nrf52840)),
      );

      expect(find.text('GPIO Pin (0–47)'), findsOneWidget);
      expect(find.text('Enter the nRF52840 GPIO pin number'), findsOneWidget);
    });

    testWidgets('shows help icon with tooltip trigger', (tester) async {
      final ctrl = TextEditingController();
      await tester.pumpWidget(_wrap(PinField(controller: ctrl)));

      expect(find.byIcon(Icons.help_outline), findsOneWidget);
    });
  });
}
