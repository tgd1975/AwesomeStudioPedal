---
id: TASK-126
title: Profile Configurator UI (Port of Web Config-Builder)
status: open
opened: 2026-04-17
effort: Large (>8h)
complexity: Senior
human-in-loop: Support
group: MobileApp
order: 5
prerequisites: [TASK-123]
---

## Description

Build the core profile editing UI — a Flutter port of `docs/tools/config-builder/`. This is the largest UI task in Group E. All screens must handle the new action types introduced in this sprint (LongPress, DoublePress, Macro).

## Acceptance Criteria

### Screens
- [ ] `HomeScreen` — three cards: "Connect to pedal", "Edit profiles", "Upload"; navigation hub
- [ ] `ProfileListScreen` — list profiles with name/description; add/remove/reorder; validation banner (green "Valid" / red error count)
- [ ] `ProfileEditorScreen` — button slots (A–D, expandable per hardware config); tap slot to open `ActionEditorScreen`
- [ ] `ActionEditorScreen` — full action configurator for one slot
- [ ] `JsonPreviewScreen` — live JSON preview, syntax-highlighted, with Copy and Share buttons

### Widgets in `ActionEditorScreen`
- [ ] `ActionTypeDropdown` — all action types from `key_lookup.cpp` `ACTION_TYPE_TABLE`, plus LongPress/DoublePress/Macro groupings
- [ ] `KeyValueField` — text input with autocomplete from `KEY_NAMES` / `MEDIA_KEY_VALUES` (ported from `builder.js`)
- [ ] `PinField` — numeric input, range 0–39, with GPIO diagram tooltip
- [ ] `DelayedActionWidget` — `delayMs` field + nested `ActionEditorScreen` as expandable card
- [ ] `MacroStepList` — reorderable list of steps; each step is a reorderable list of `ActionEditorScreen` cards
- [ ] `LongPressSlot` / `DoublePressSlot` — optional collapsible cards with nested `ActionEditorScreen`

### Validation
- [ ] Persistent validation banner at bottom of `ProfileListScreen`
- [ ] Tapping an error navigates to the offending field

## Files to Touch

- `app/lib/screens/home_screen.dart` (new)
- `app/lib/screens/profile_list_screen.dart` (new)
- `app/lib/screens/profile_editor_screen.dart` (new)
- `app/lib/screens/action_editor_screen.dart` (new)
- `app/lib/screens/json_preview_screen.dart` (new)
- `app/lib/widgets/` (new directory with widget files)

## Test Plan

**Widget tests**: covered by TASK-130.
Manual: open app → create a profile → add a macro action with two steps → preview JSON → verify output matches expected structure.

## Prerequisites

- **TASK-123** — Flutter scaffold and navigation must exist

## Notes

Reference `docs/tools/config-builder/builder.js` for the action type list, key name autocomplete values, and existing UI logic. Match the web builder's colour scheme exactly (`#2563eb` accent). The web builder's `ACTION_TYPES` array should be ported to a Dart constant in `app/lib/constants/action_types.dart`.
