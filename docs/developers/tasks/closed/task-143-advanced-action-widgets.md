---
id: TASK-143
title: Profile Configurator UI — Advanced Action Widgets
status: closed
closed: 2026-04-18
opened: 2026-04-18
effort: Medium (2-8h)
complexity: Senior
human-in-loop: No
group: MobileApp
order: 7
prerequisites: [TASK-126]
---

## Description

Implement the three advanced nested action widgets for `ActionEditorScreen`: Delayed actions,
Macro step lists, and LongPress/DoublePress slots. All three involve recursive nesting of
`ActionEditorScreen` inside a parent widget, which is the primary complexity.

## Acceptance Criteria

- [ ] `DelayedActionWidget` — `delayMs` numeric field + nested `ActionEditorScreen` as an
  expandable card; selecting "Delayed" action type in `ActionTypeDropdown` reveals this widget
- [ ] `MacroStepList` — reorderable list of steps; each step is a reorderable list of
  `ActionEditorScreen` cards; add/remove step and add/remove action within step
- [ ] `LongPressSlot` — optional collapsible card with a nested `ActionEditorScreen`; shown
  on any button slot in `ProfileEditorScreen`
- [ ] `DoublePressSlot` — same structure as `LongPressSlot`
- [ ] `ActionTypeDropdown` updated: Delayed/Macro/LongPress/DoublePress entries now fully
  enabled (remove "coming soon" tooltip added in TASK-126)
- [ ] `PinField` updated: GPIO diagram tooltip added (deferred from TASK-126)
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/widgets/delayed_action_widget.dart` (new)
- `app/lib/widgets/macro_step_list.dart` (new)
- `app/lib/widgets/long_press_slot.dart` (new)
- `app/lib/widgets/double_press_slot.dart` (new)
- `app/lib/widgets/action_type_dropdown.dart` — remove "coming soon" stubs
- `app/lib/widgets/pin_field.dart` — add GPIO diagram tooltip

## Test Plan

Widget tests covered by TASK-130: selecting "Macro" shows `MacroStepList`; selecting
"Delayed" shows `DelayedActionWidget` with nested editor.
Manual: create a macro action with two steps, verify JSON preview (TASK-144) shows correct
nested structure.

## Prerequisites

- **TASK-126** — core screens and `ActionEditorScreen` shell must exist
