import 'package:awesome_studio_pedal/models/action_config.dart';
import 'package:awesome_studio_pedal/models/macro_step.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('ActionConfig round-trip', () {
    test('SendKeyAction', () {
      final a =
          ActionConfig(type: 'SendKeyAction', value: 'KEY_F5', name: 'F5');
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('SendMediaKeyAction', () {
      final a =
          ActionConfig(type: 'SendMediaKeyAction', value: 'MEDIA_PLAY_PAUSE');
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('SendStringAction', () {
      final a = ActionConfig(type: 'SendStringAction', value: 'hello world');
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('PinHighAction', () {
      final a = ActionConfig(type: 'PinHighAction', pin: 13);
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('PinLowAction', () {
      final a = ActionConfig(type: 'PinLowAction', pin: 12);
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('PinToggleAction', () {
      final a = ActionConfig(type: 'PinToggleAction', pin: 27);
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('PinHighWhilePressedAction', () {
      final a = ActionConfig(type: 'PinHighWhilePressedAction', pin: 14);
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('PinLowWhilePressedAction', () {
      final a = ActionConfig(type: 'PinLowWhilePressedAction', pin: 14);
      expect(ActionConfig.fromJson(a.toJson()).toJson(), equals(a.toJson()));
    });

    test('DelayedAction with nested action', () {
      final nested = ActionConfig(type: 'SendKeyAction', value: 'KEY_ENTER');
      final a =
          ActionConfig(type: 'DelayedAction', delayMs: 200, action: nested);
      final roundTripped = ActionConfig.fromJson(a.toJson());
      expect(roundTripped.delayMs, equals(200));
      expect(roundTripped.action?.type, equals('SendKeyAction'));
      expect(roundTripped.action?.value, equals('KEY_ENTER'));
    });

    test('MacroAction with steps', () {
      final step1 = MacroStep(actions: [
        ActionConfig(type: 'SendKeyAction', value: 'KEY_CTRL'),
        ActionConfig(type: 'SendKeyAction', value: 'KEY_C'),
      ]);
      final a = ActionConfig(type: 'MacroAction', steps: [step1]);
      final roundTripped = ActionConfig.fromJson(a.toJson());
      expect(roundTripped.type, equals('MacroAction'));
      expect(roundTripped.steps?.length, equals(1));
      expect(roundTripped.steps![0].actions.length, equals(2));
      expect(roundTripped.steps![0].actions[0].value, equals('KEY_CTRL'));
    });

    test('ActionConfig with longPress and doublePress', () {
      final lp = ActionConfig(type: 'SendKeyAction', value: 'KEY_ESC');
      final dp = ActionConfig(type: 'SendMediaKeyAction', value: 'MEDIA_MUTE');
      final a = ActionConfig(
        type: 'SendKeyAction',
        value: 'KEY_SPACE',
        longPress: lp,
        doublePress: dp,
      );
      final rt = ActionConfig.fromJson(a.toJson());
      expect(rt.longPress?.value, equals('KEY_ESC'));
      expect(rt.doublePress?.value, equals('MEDIA_MUTE'));
    });
  });
}
