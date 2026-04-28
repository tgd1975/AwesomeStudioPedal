---
id: TASK-281
title: Defect — Validation banner stale until profile count changes
status: closed
closed: 2026-04-28
opened: 2026-04-28
effort: XS (<30m)
effort_actual: XS (<30m)
complexity: Junior
human-in-loop: Clarification
epic: feature_test
order: 5
---

## Description

Surfaced while verifying TASK-264 + TASK-265 on 2026-04-27. After
fixing `value: "NOT_A_VALID_KEY_VALUE"` → `value: "KEY_PAGE_UP"` in
the Action Editor and tapping Save, the Profile List banner kept
reading "1 error" until the user backed out to Home and re-entered
Profile List, at which point it correctly turned green.

The current trigger logic in
[app/lib/widgets/validation_banner.dart](../../../../app/lib/widgets/validation_banner.dart)
revalidates only when the **count** of profiles changes:

```dart
if (count != _lastProfileCount) {
  _lastProfileCount = count;
  WidgetsBinding.instance.addPostFrameCallback((_) => _validate(state));
}
```

This was fine when validation was schema-only — a missing field can
only appear / disappear when a profile is added / removed. With the
TASK-264 runtime-resolvability check, an action's `value` can flip
between resolvable and unresolvable without changing the profile
count, so the banner becomes stale. See
[idea-045](../../ideas/archived/idea-045-validation-banner-revalidates-on-action-edit.md)
for the full motivation and the suggested fix (subscribe to the
`ProfilesState` listenable and revalidate on every notification, with
optional ~250 ms debounce if validation is expensive on large
profile sets).

## Acceptance Criteria

- [ ] Editing an action's value in the Action Editor and tapping Save
      causes the Profile List validation banner to refresh on the
      next frame (no need to navigate away and back).
- [ ] The banner does not regress on profile-add / profile-remove —
      both still trigger revalidation as before.
- [ ] If a debounce is added, it is justified by a measurement (or a
      one-line note in the diff) — do not add it speculatively.

## Test Plan

**Host tests** (Flutter widget tests via the app's test runner):

- Add a widget test that mounts the Profile List with a `ProfilesState`
  containing one runtime-invalid action, asserts the banner reads
  "1 error", mutates the action's value via `ProfilesState` (without
  changing the profile count), pumps a frame, and asserts the banner
  has cleared. This is the regression guard.
- Confirm the existing profile-add / profile-remove banner-refresh
  paths still pass.

**On-device verification** (no new automated coverage required):

- Re-run the TASK-264 / TASK-265 verification flow on a Pixel 9: open
  a profile with a known-bad action value, fix it in the Action
  Editor, tap Save, and confirm the banner clears without leaving the
  Profile List.

## Notes

- Implementation approach decision: pick one of (a) subscribe to
  `ProfilesState` in `didChangeDependencies` and revalidate on every
  notification, or (b) the same with a ~250 ms debounce. Idea-045
  recommends starting with (a) and only adding the debounce if a
  measurement says it's necessary.
- Remember to remove the listener in `dispose` to avoid leaks.
- Cross-references: TASK-264 (validator coverage of unresolvable
  values, closed), TASK-265 (Action Editor crash on the same input,
  closed). Both surfaced this defect during their on-device
  verification round.
