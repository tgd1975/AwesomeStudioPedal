---
id: TASK-253
title: Defect — UI overflow in landscape & Media Key dropdown; Media Key filter not implemented
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (1-3h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 30
---

## Summary

Three related UI/UX defects found while running TASK-153 (H-03) and
TASK-155 (AE-04).

### Sub-defect A — Home screen: landscape rotation overflows by 57 px (H-03)

Rotating the home screen to landscape triggers a Flutter
`BOTTOM OVERFLOWED BY 57 PIXELS` render warning; the Upload card is
partially clipped. The Connect/Edit/Community cards remain visible.
Likely fix: wrap the home content in a scrollable (`SingleChildScrollView`
or `ListView`) so overflowing landscape layouts can scroll instead of
clipping.

### Sub-defect B — Media Key value field overflows by 79 px (AE-04)

After selecting Action Type "Media Key" and a value such as MEDIA_VOLUME_UP,
the value-field shows the Flutter `OVERFLOWED BY 79 PIXELS` indicator on the
right edge. The dropdown chevron is partially obscured. Likely fix: tighten
the field's intrinsic width or constrain the text style to avoid hard
overflow when the longest media-key label is rendered.

### Sub-defect C — Media Key dropdown does not filter on type (AE-04)

Test plan AE-04 says "type 'PLAY' filters results" — but in practice the
Media Key value field opens a *read-only* autocomplete dropdown listing
**all** media keys, with no IME ever opening, so users cannot type to
filter. Compare with "Key (named)" (AE-02) where typing `KEY_F` correctly
narrows the suggestions to KEY_F1..KEY_F12. The two action types should
behave consistently.

## Tests affected

- TASK-153 H-03 (overflow in landscape).
- TASK-155 AE-04 (Media Key autocomplete + filter).

## Acceptance Criteria

- [x] Home screen in landscape no longer shows the overflow indicator;
  Upload card is fully visible (or scrollable to).
- [x] Media Key value field no longer overflows in any selected state.
- [x] Media Key dropdown supports type-to-filter the same way Key (named)
  does, with the soft IME opening on field tap.

## Resolution

### Sub-defect A — Home landscape overflow (H-03)

The home `body:` was a `Padding(child: Column(...))` with four cards plus
spacers. In landscape that exceeds the viewport height, hence the
57 px clip indicator. Wrapped the body in `SingleChildScrollView` so the
column scrolls when needed and stays static when it fits.
[app/lib/screens/home_screen.dart](app/lib/screens/home_screen.dart)

### Sub-defects B + C — Media Key field overflow + missing filter

Both stem from the same cause: Media Key used a `DropdownButtonFormField`
with all 27 entries, while the visually similar Key (named) field used an
`Autocomplete<String>`. The dropdown widget has an intrinsic width that
exceeds the action editor's available width when the longest media-key
label (`KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION`) is chosen — that's
the 79 px clip. It also has no IME — the chevron just lists all options
read-only — so `AE-04` "type 'PLAY' filters results" couldn't work.

Replaced the Media Key dropdown with the same autocomplete pattern.
While there, factored the duplicated autocomplete into a single
private `_NameAutocomplete` widget (`label`, `options`, `controller`)
so Media Key and Key (named) share one implementation.
[app/lib/widgets/key_value_field.dart](app/lib/widgets/key_value_field.dart)

### Coverage

Added [app/test/widget/key_value_field_test.dart](app/test/widget/key_value_field_test.dart)
with 4 widget tests:

1. Media Key uses an autocomplete TextField, not a Dropdown
   (regression guard against silently reverting to the dropdown).
2. Media Key field filters options when the user types `PLAY`.
3. Key (named) variant filters via the same autocomplete with `KEY_F`.
4. Raw `SendKeyAction` shows a plain text field with the HID-code hint.

Suite is now 62 → 66 tests. `flutter analyze` clean.

The numeric overflow values (57 px / 79 px) cannot be re-asserted in
widget tests without pinning the viewport to a specific landscape phone
geometry. Their root causes are removed; the on-device verification
will happen during TASK-153 (H-03) and TASK-155 (AE-04) resume.
