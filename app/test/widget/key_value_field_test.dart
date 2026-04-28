import 'package:awesome_studio_pedal/constants/action_types.dart';
import 'package:awesome_studio_pedal/widgets/key_value_field.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

Widget _host(Widget child) => MaterialApp(home: Scaffold(body: child));

void main() {
  group('KeyValueField', () {
    testWidgets('Media Key uses an autocomplete TextField, not a Dropdown',
        (tester) async {
      final ctrl = TextEditingController();
      await tester.pumpWidget(_host(
        KeyValueField(type: 'SendMediaKeyAction', controller: ctrl),
      ));
      expect(find.byType(TextField), findsOneWidget);
      expect(find.byType(DropdownButtonFormField<String>), findsNothing);
    });

    testWidgets('Media Key field filters options when the user types',
        (tester) async {
      final ctrl = TextEditingController();
      await tester.pumpWidget(_host(
        KeyValueField(type: 'SendMediaKeyAction', controller: ctrl),
      ));
      await tester.enterText(find.byType(TextField), 'PLAY');
      await tester.pumpAndSettle();
      expect(find.text('MEDIA_PLAY_PAUSE'), findsOneWidget);
      expect(find.text('MEDIA_VOLUME_UP'), findsNothing);
    });

    testWidgets('Key (named) variant filters via the same autocomplete',
        (tester) async {
      final ctrl = TextEditingController();
      await tester.pumpWidget(_host(
        KeyValueField(type: kKeyNamedSentinel, controller: ctrl),
      ));
      await tester.enterText(find.byType(TextField), 'KEY_F');
      await tester.pumpAndSettle();
      expect(find.text('KEY_F1'), findsOneWidget);
      expect(find.text('KEY_PAGE_UP'), findsNothing);
    });

    testWidgets(
        'SendKeyAction (raw HID) shows a plain text field with a non-misleading '
        'example — TASK-277. The previous example "0x28" was the USB HID Usage '
        'ID for Enter, but the firmware uses internal codes (Enter = 0xB0); a '
        'user who copied 0x28 directly got nothing on the host. The example '
        'must be a code that actually does what it claims.', (tester) async {
      final ctrl = TextEditingController();
      await tester.pumpWidget(_host(
        KeyValueField(type: 'SendKeyAction', controller: ctrl),
      ));
      expect(find.byType(TextField), findsOneWidget);
      // Label uses the firmware-internal code 0xB0 (Enter), and a helperText
      // disambiguates from USB HID Usage IDs.
      expect(find.textContaining('0xB0'), findsOneWidget);
      expect(find.textContaining('Firmware-internal'), findsOneWidget);
      // Guard against regression: 0x28 must not appear as the leading example
      // anywhere in the field's chrome.
      expect(find.textContaining('0x28)'), findsNothing,
          reason: 'Hint must not lead with 0x28 — it is the USB HID Usage ID '
              'for Enter, but the firmware\'s internal code for Enter is 0xB0. '
              'See TASK-277 / idea-039.');
    });
  });
}
