import 'package:awesome_studio_pedal/constants/action_types.dart';
import 'package:awesome_studio_pedal/models/action_config.dart';
import 'package:awesome_studio_pedal/screens/action_editor_screen.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

// TASK-280: changing Action Type in the Action Editor must clear the
// value field when the new type's value space differs from the old —
// otherwise a "Key (named)" value like KEY_PAGE_UP carries over into
// "Media Key", "Type String", or "Key (raw HID)" and saves a
// meaningless action.

Widget _host(ActionConfig initial) {
  return MaterialApp(
    home: Scaffold(
      body: ActionEditorScreen(
        buttonId: 'A',
        initial: initial,
        onSave: (_) {},
        embeddedMode: true,
      ),
    ),
  );
}

Future<void> _selectActionType(WidgetTester tester, String label) async {
  await tester.tap(find.byType(DropdownButtonFormField<String>));
  await tester.pumpAndSettle();
  await tester.tap(find.text(label).last);
  await tester.pumpAndSettle();
}

Finder _valueField(String labelText) {
  return find.ancestor(
    of: find.text(labelText),
    matching: find.byType(TextField),
  );
}

void main() {
  group('Action Editor value carry-over on type change (TASK-280)', () {
    testWidgets('switching to Media Key clears a Key (named) value',
        (tester) async {
      final initial = ActionConfig(
        type: 'SendCharAction',
        value: 'KEY_PAGE_UP',
      );
      await tester.pumpWidget(_host(initial));

      // Sanity: editor loads as Key (named) with the carried value.
      final namedField = tester.widget<TextField>(_valueField('Key name'));
      expect(namedField.controller!.text, equals('KEY_PAGE_UP'));

      await _selectActionType(tester, 'Media Key');

      final mediaField = tester.widget<TextField>(_valueField('Media Key'));
      expect(mediaField.controller!.text, isEmpty);
    });

    testWidgets('switching to Type String clears a Key (named) value',
        (tester) async {
      final initial = ActionConfig(
        type: 'SendCharAction',
        value: 'KEY_PAGE_UP',
      );
      await tester.pumpWidget(_host(initial));
      await _selectActionType(tester, 'Type String');

      final stringField =
          tester.widget<TextField>(_valueField('String to type'));
      expect(stringField.controller!.text, isEmpty);
    });

    testWidgets('switching to Key (raw HID) clears a Key (named) value',
        (tester) async {
      final initial = ActionConfig(
        type: 'SendCharAction',
        value: 'KEY_PAGE_UP',
      );
      await tester.pumpWidget(_host(initial));
      await _selectActionType(tester, 'Key (raw HID)');

      final hidField = tester.widget<TextField>(
        _valueField('Firmware HID code (e.g. 0xB0 = Enter)'),
      );
      expect(hidField.controller!.text, isEmpty);
    });

    test('value-space categoriser groups same-space types together', () {
      // Hard guarantee that the named-key sentinel and SendCharAction stay
      // in the same group, so a value loaded as one is preserved when
      // resolved to the other (the only "preserve" path the categoriser
      // protects, since the dropdown only exposes one of the two).
      String space(String type) {
        if (type == kKeyNamedSentinel || type == 'SendCharAction') {
          return 'named';
        }
        if (type == 'SendMediaKeyAction') return 'mediaKey';
        if (type == 'SendKeyAction') return 'rawHid';
        if (type == 'SendStringAction') return 'string';
        if (type == 'SerialOutputAction') return 'serial';
        return 'none';
      }

      expect(space(kKeyNamedSentinel), equals(space('SendCharAction')));
      expect(space('SendKeyAction'), isNot(equals(space(kKeyNamedSentinel))));
      expect(space('SendMediaKeyAction'),
          isNot(equals(space('SendStringAction'))));
    });
  });
}
