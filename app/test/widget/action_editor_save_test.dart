import 'package:awesome_studio_pedal/models/action_config.dart';
import 'package:awesome_studio_pedal/models/macro_step.dart';
import 'package:awesome_studio_pedal/screens/action_editor_screen.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:go_router/go_router.dart';

// Regression coverage for TASK-276: re-saving a primary action through
// ActionEditorScreen must preserve sibling/child fields the editor does
// not expose in its UI (longPress, doublePress, MacroAction steps,
// DelayedAction nested action). Before the fix, _save() constructed a
// fresh ActionConfig with only type/value/pin/name and silently dropped
// the rest.

Widget _host(ActionConfig initial, void Function(ActionConfig) onSave) {
  final router = GoRouter(
    initialLocation: '/',
    routes: [
      GoRoute(
        path: '/',
        builder: (_, __) => Scaffold(
          body: ActionEditorScreen(
            buttonId: 'A',
            initial: initial,
            onSave: onSave,
            embeddedMode: true,
          ),
        ),
      ),
    ],
  );
  return MaterialApp.router(routerConfig: router);
}

void main() {
  group('ActionEditorScreen save preserves unedited sibling fields', () {
    testWidgets('longPress is preserved when only the primary name changes',
        (tester) async {
      final initial = ActionConfig(
        type: 'SendCharAction',
        value: 'KEY_PAGE_UP',
        name: 'Prev Page',
        longPress: ActionConfig(
          type: 'SendCharAction',
          value: 'KEY_HOME',
          name: 'First Page',
        ),
      );
      ActionConfig? saved;
      await tester.pumpWidget(_host(initial, (a) => saved = a));

      await tester.enterText(
        find.widgetWithText(TextField, 'Prev Page'),
        'Next Page',
      );
      await tester.tap(find.widgetWithText(FilledButton, 'Apply'));
      await tester.pumpAndSettle();

      expect(saved, isNotNull);
      expect(saved!.name, equals('Next Page'));
      expect(saved!.longPress, isNotNull);
      expect(saved!.longPress!.type, equals('SendCharAction'));
      expect(saved!.longPress!.value, equals('KEY_HOME'));
      expect(saved!.longPress!.name, equals('First Page'));
    });

    testWidgets('doublePress is preserved across re-save', (tester) async {
      final initial = ActionConfig(
        type: 'SendCharAction',
        value: 'KEY_PAGE_DOWN',
        doublePress: ActionConfig(
          type: 'SendMediaKeyAction',
          value: 'MEDIA_MUTE',
        ),
      );
      ActionConfig? saved;
      await tester.pumpWidget(_host(initial, (a) => saved = a));
      await tester.tap(find.widgetWithText(FilledButton, 'Apply'));
      await tester.pumpAndSettle();

      expect(saved!.doublePress, isNotNull);
      expect(saved!.doublePress!.value, equals('MEDIA_MUTE'));
    });

    testWidgets('MacroAction steps survive re-save through editor',
        (tester) async {
      final initial = ActionConfig(
        type: 'MacroAction',
        steps: [
          MacroStep(actions: [
            ActionConfig(type: 'SendKeyAction', value: 'KEY_CTRL'),
            ActionConfig(type: 'SendKeyAction', value: 'KEY_C'),
          ]),
        ],
      );
      ActionConfig? saved;
      await tester.pumpWidget(_host(initial, (a) => saved = a));
      await tester.tap(find.widgetWithText(FilledButton, 'Apply'));
      await tester.pumpAndSettle();

      expect(saved!.steps, isNotNull);
      expect(saved!.steps!.length, equals(1));
      expect(saved!.steps![0].actions.length, equals(2));
      expect(saved!.steps![0].actions[0].value, equals('KEY_CTRL'));
    });

    testWidgets('DelayedAction nested action survives re-save', (tester) async {
      final initial = ActionConfig(
        type: 'DelayedAction',
        delayMs: 200,
        action: ActionConfig(type: 'SendKeyAction', value: 'KEY_ENTER'),
      );
      ActionConfig? saved;
      await tester.pumpWidget(_host(initial, (a) => saved = a));
      await tester.tap(find.widgetWithText(FilledButton, 'Apply'));
      await tester.pumpAndSettle();

      expect(saved!.action, isNotNull);
      expect(saved!.action!.type, equals('SendKeyAction'));
      expect(saved!.action!.value, equals('KEY_ENTER'));
    });
  });
}
