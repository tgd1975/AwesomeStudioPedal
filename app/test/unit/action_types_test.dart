import 'package:awesome_studio_pedal/constants/action_types.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('savedActionType', () {
    test('translates the named-key sentinel to SendKeyAction', () {
      expect(savedActionType(kKeyNamedSentinel), 'SendKeyAction');
    });

    test('passes regular action types through unchanged', () {
      expect(savedActionType('SendKeyAction'), 'SendKeyAction');
      expect(savedActionType('SendMediaKeyAction'), 'SendMediaKeyAction');
      expect(savedActionType('SendStringAction'), 'SendStringAction');
      expect(savedActionType('PinHighAction'), 'PinHighAction');
    });
  });

  group('editorActionType', () {
    test('SendKeyAction with a KEY_* value surfaces as the named sentinel', () {
      expect(
          editorActionType('SendKeyAction', 'KEY_PAGE_UP'), kKeyNamedSentinel);
    });

    test('SendKeyAction with a hex value surfaces as raw HID', () {
      expect(editorActionType('SendKeyAction', '0x28'), 'SendKeyAction');
    });

    test('legacy SendCharAction with a KEY_* value surfaces as named', () {
      expect(
          editorActionType('SendCharAction', 'KEY_PAGE_UP'), kKeyNamedSentinel);
    });

    test(
        'SendCharAction with a single character falls back to (none) — the '
        'dropdown has no SendCharAction item; opening the editor on it '
        'would otherwise crash with a Flutter dropdown assertion. See '
        'TASK-265.', () {
      expect(editorActionType('SendCharAction', 'a'), '');
    });

    test(
        'SendCharAction with an unresolvable non-KEY_*, non-single-char value '
        'surfaces as the named-key sentinel so the editor renders normally — '
        'TASK-265.', () {
      expect(editorActionType('SendCharAction', 'NOT_A_VALID_KEY_VALUE'),
          kKeyNamedSentinel);
    });

    test(
        'unknown action types fall back to (none) so the dropdown invariant '
        'holds — TASK-265.', () {
      expect(editorActionType('NotARealAction', 'x'), '');
    });

    test('null type yields empty string (new action)', () {
      expect(editorActionType(null, null), '');
    });
  });
}
