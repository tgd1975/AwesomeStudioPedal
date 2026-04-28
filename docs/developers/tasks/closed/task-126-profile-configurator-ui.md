---
id: TASK-126
title: Profile Configurator UI — Core Screens and Basic Action Editor
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Senior
human-in-loop: Support
epic: MobileApp
order: 6
prerequisites: [TASK-123]
---

## Description

Build the core profile editing screens and a basic `ActionEditorScreen` covering simple
action types (key, media key, string, pin, serial). Advanced nested widgets (Delayed,
Macro, LongPress, DoublePress) are implemented separately in TASK-143. The JSON preview
screen and validation banner are in TASK-144.

## Acceptance Criteria

### Screens

- [ ] `HomeScreen` — three cards: "Connect to pedal", "Edit profiles", "Upload"; navigation hub
- [ ] `ProfileListScreen` — list profiles with name/description; add/remove/reorder
- [ ] `ProfileEditorScreen` — button slots (A–D, expandable per hardware config); tap slot to
  open `ActionEditorScreen`
- [ ] `ActionEditorScreen` — action configurator for one slot covering simple types

### Widgets in `ActionEditorScreen` (basic)

- [ ] `ActionTypeDropdown` — all action types from `key_lookup.cpp` `ACTION_TYPE_TABLE`
  (SendKey, SendMediaKey, SendString, PinHigh, PinLow, PinToggle, PinHighWhilePressed,
  PinLowWhilePressed, SerialOutput); Delayed/Macro/LongPress/DoublePress shown but disabled
  with "coming soon" tooltip until TASK-143
- [ ] `KeyValueField` — text input with autocomplete from `KEY_NAMES` / `MEDIA_KEY_VALUES`
  (ported from `builder.js`)
- [ ] `PinField` — numeric input, range 0–39
- [ ] `NameField` — optional display name for the action

### Style

- [ ] Dark/light `MaterialApp` theme with accent `#2563eb`, background `#f5f5f5` (matches web
  config-builder)
- [ ] `flutter analyze` passes

## Files to Touch

- `app/lib/screens/home_screen.dart` (new)
- `app/lib/screens/profile_list_screen.dart` (new)
- `app/lib/screens/profile_editor_screen.dart` (new)
- `app/lib/screens/action_editor_screen.dart` (new)
- `app/lib/widgets/action_type_dropdown.dart` (new)
- `app/lib/widgets/key_value_field.dart` (new)
- `app/lib/widgets/pin_field.dart` (new)
- `app/lib/constants/action_types.dart` (new — port of `builder.js` ACTION_TYPES array)

## Test Plan

Widget tests covered by TASK-130. Manual: open app → create profile → assign SendKey action
to a button slot → navigate back → profile appears in list.

## Prerequisites

- **TASK-123** — Flutter scaffold and navigation must exist

## Notes

Reference `docs/tools/config-builder/builder.js` for the `ACTION_TYPES` array, `KEY_NAMES`,
`MEDIA_KEY_VALUES`, and colour scheme. Port `ACTION_TYPES` to
`app/lib/constants/action_types.dart`. GPIO diagram tooltip on `PinField` is deferred to
TASK-143.
