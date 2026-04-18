import 'package:awesome_studio_pedal/constants/action_types.dart';
import 'package:awesome_studio_pedal/widgets/action_type_dropdown.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('ActionTypeDropdown', () {
    testWidgets('renders all action types', (tester) async {
      String selected = '';

      await tester.pumpWidget(
        MaterialApp(
          home: Scaffold(
            body: ActionTypeDropdown(
              value: selected,
              onChanged: (v) => selected = v,
            ),
          ),
        ),
      );

      // Open the dropdown.
      await tester.tap(find.byType(DropdownButtonFormField<String>));
      await tester.pumpAndSettle();

      // Verify all action type labels appear.
      for (final opt in kActionTypes) {
        expect(find.text(opt.label), findsWidgets);
      }
    });

    testWidgets('selecting an action type calls onChanged', (tester) async {
      String? changed;

      await tester.pumpWidget(
        MaterialApp(
          home: Scaffold(
            body: ActionTypeDropdown(
              value: '',
              onChanged: (v) => changed = v,
            ),
          ),
        ),
      );

      await tester.tap(find.byType(DropdownButtonFormField<String>));
      await tester.pumpAndSettle();

      await tester.tap(find.text('Media Key').last);
      await tester.pumpAndSettle();

      expect(changed, equals('SendMediaKeyAction'));
    });
  });
}
