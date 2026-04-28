import 'package:awesome_studio_pedal/services/action_value_resolver.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('ActionValueResolver.resolve', () {
    test('SendKeyAction with a known named key resolves', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendKeyAction', value: 'KEY_PAGE_UP');
      expect(r.isResolvable, isTrue);
    });

    test('SendKeyAction with a hex code in [1, 255] resolves', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendKeyAction', value: '0xB0');
      expect(r.isResolvable, isTrue);
    });

    test('SendKeyAction with a decimal in [1, 255] resolves', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendKeyAction', value: '176');
      expect(r.isResolvable, isTrue);
    });

    test('SendKeyAction with an unresolvable name fails with a clear reason',
        () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendKeyAction', value: 'NOT_A_VALID_KEY_VALUE');
      expect(r.isResolvable, isFalse);
      expect(r.reason, contains('NOT_A_VALID_KEY_VALUE'));
    });

    test('SendKeyAction with hex out of range fails', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendKeyAction', value: '0xFFFF');
      expect(r.isResolvable, isFalse);
    });

    test('SendMediaKeyAction with a known media key resolves', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendMediaKeyAction', value: 'MEDIA_PLAY_PAUSE');
      expect(r.isResolvable, isTrue);
    });

    test('SendMediaKeyAction with a regular key name fails', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendMediaKeyAction', value: 'KEY_PAGE_UP');
      expect(r.isResolvable, isFalse);
    });

    test('SendCharAction with a single character resolves', () {
      final r =
          ActionValueResolver.resolve(actionType: 'SendCharAction', value: 'a');
      expect(r.isResolvable, isTrue);
    });

    test('SendCharAction with a known named key resolves (legacy)', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendCharAction', value: 'KEY_PAGE_UP');
      expect(r.isResolvable, isTrue);
    });

    test('SendCharAction with an unresolvable value fails', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendCharAction', value: 'NOT_A_VALID_KEY_VALUE');
      expect(r.isResolvable, isFalse);
    });

    test('SendStringAction accepts any non-empty string', () {
      final r = ActionValueResolver.resolve(
          actionType: 'SendStringAction', value: 'ctrl+z');
      expect(r.isResolvable, isTrue);
    });

    test('Pin actions resolve regardless of value', () {
      final r =
          ActionValueResolver.resolve(actionType: 'PinHighAction', value: null);
      expect(r.isResolvable, isTrue);
    });

    test('Empty value fails for value-bearing types', () {
      final r =
          ActionValueResolver.resolve(actionType: 'SendKeyAction', value: '');
      expect(r.isResolvable, isFalse);
    });
  });

  group('ActionValueResolver.findUnresolvable', () {
    test('TASK-264 reproducer: SendCharAction with NOT_A_VALID_KEY_VALUE', () {
      final json = {
        'profiles': [
          {
            'name': '01 Score Navigator',
            'buttons': {
              'A': {
                'type': 'SendCharAction',
                'name': 'Prev Page',
                'value': 'NOT_A_VALID_KEY_VALUE',
              },
            },
          },
        ],
      };
      final errors = ActionValueResolver.findUnresolvable(json);
      expect(errors, hasLength(1));
      expect(errors.first, contains('/profiles/0/buttons/A/value'));
      expect(errors.first, contains('NOT_A_VALID_KEY_VALUE'));
    });

    test('valid profiles produce no errors', () {
      final json = {
        'profiles': [
          {
            'name': '01 Score Navigator',
            'buttons': {
              'A': {
                'type': 'SendCharAction',
                'value': 'KEY_PAGE_UP',
              },
              'B': {
                'type': 'SendKeyAction',
                'value': '0xB0',
              },
              'C': {
                'type': 'SendStringAction',
                'value': 'ctrl+z',
              },
              'D': {
                'type': 'PinHighAction',
                'pin': 27,
              },
            },
          },
        ],
      };
      expect(ActionValueResolver.findUnresolvable(json), isEmpty);
    });

    test('walks longPress sub-actions', () {
      final json = {
        'profiles': [
          {
            'buttons': {
              'A': {
                'type': 'SendCharAction',
                'value': 'KEY_PAGE_UP',
                'longPress': {
                  'type': 'SendCharAction',
                  'value': 'BAD_LONGPRESS',
                },
              },
            },
          },
        ],
      };
      final errors = ActionValueResolver.findUnresolvable(json);
      expect(errors, hasLength(1));
      expect(errors.first, contains('/profiles/0/buttons/A/longPress/value'));
    });

    test('walks DelayedAction.action', () {
      final json = {
        'profiles': [
          {
            'buttons': {
              'A': {
                'type': 'DelayedAction',
                'delayMs': 100,
                'action': {
                  'type': 'SendKeyAction',
                  'value': 'NOT_RESOLVABLE',
                },
              },
            },
          },
        ],
      };
      final errors = ActionValueResolver.findUnresolvable(json);
      expect(errors, hasLength(1));
      expect(errors.first, contains('/profiles/0/buttons/A/action/value'));
    });

    test('walks MacroAction.steps', () {
      final json = {
        'profiles': [
          {
            'buttons': {
              'A': {
                'type': 'MacroAction',
                'steps': [
                  [
                    {'type': 'SendKeyAction', 'value': 'BAD_KEY'},
                  ],
                  [
                    {'type': 'PinHighAction', 'pin': 27},
                  ],
                ],
              },
            },
          },
        ],
      };
      final errors = ActionValueResolver.findUnresolvable(json);
      expect(errors, hasLength(1));
      expect(errors.first, contains('/profiles/0/buttons/A/steps/0/0/value'));
    });
  });
}
