---
id: TASK-256
title: Defect — Profile List accessibility (empty content-desc on FAB and row icons)
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (1-3h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Low
human-in-loop: Main
epic: feature_test
order: 32
---

## Summary

While running TASK-154 (PL-01..PL-13), the UI dump for the Profile List
screen showed multiple interactive controls with **empty `content-desc`**
attributes — meaning TalkBack and other Android accessibility services
have nothing to announce when these controls are focused. Affected
elements observed:

- The **Add Profile (+) FAB** in the bottom-right of the list — clickable
  but `content-desc=""`.
- The **trash icons** on each profile row — clickable but
  `content-desc=""`.
- The **reorder handles** on each profile row — clickable but
  `content-desc=""`.

In contrast the screen's app-bar buttons (Import JSON / Export JSON /
JSON Preview) and the profile rows themselves do carry meaningful
content-desc strings, so this is a localised regression on the icon
buttons rather than a screen-wide gap.

This defect was found via `uiautomator dump` while driving the screen
under adb (the same flow used in TASK-153/154/155/156). Reproduce by
running:

```bash
adb exec-out uiautomator dump /sdcard/d.xml
```

…on the Profile List screen, and looking for `clickable="true"` nodes
with `content-desc=""`.

## Tests affected

- TASK-154: did not block any specific test ID, but contributed to PL-05
  being deferred (could not target the reorder handle reliably without an
  accessible label) and made adb-driven verification of the FAB and trash
  icon noisier than necessary.

## Likely fix

In Flutter, wrap each `IconButton` (or `FloatingActionButton`) with a
`Tooltip` or an explicit `Semantics(label: ...)` so a content description
is exposed to the accessibility tree. Suggested labels:

- FAB → "Add profile"
- Trash icon → "Delete profile `<name>`" (interpolating the row's profile name)
- Reorder handle → "Reorder profile `<name>`"

## Acceptance Criteria

- [x] FAB exposes a non-empty content-desc / tooltip such as "Add profile".
- [x] Each trash icon exposes a content-desc that includes the affected
  profile name (e.g. "Delete profile My Test Profile").
- [x] Each reorder handle exposes a content-desc that identifies the row.
- [x] A widget test covers the FAB and at least one row's trash icon to
  prevent regression.

## Resolution

[app/lib/screens/profile_list_screen.dart](app/lib/screens/profile_list_screen.dart):

- `FloatingActionButton.tooltip: 'Add profile'`. Flutter exposes the
  FAB tooltip both as the long-press hover tooltip *and* as the
  accessibility content-desc, so one prop covers both behaviours.
- `IconButton.tooltip: 'Delete profile ${profile.name}'` for the trash
  icon — interpolates the row's profile name so screen-reader users
  hear which profile they're about to delete.
- `Semantics(label: 'Reorder profile ${profile.name}', child: Icon(…))`
  wrapping the drag handle. The handle is a non-interactive `Icon`
  itself; `ReorderableListView` attaches drag detection around it, and
  the surrounding Material drag-handle semantics absorb our label —
  the test uses a regex matcher because the final composed label is
  `"Reorder profile <name>, …"`.

Coverage: [app/test/widget/profile_list_a11y_test.dart](app/test/widget/profile_list_a11y_test.dart)
with 3 widget tests, one per control. The reorder-handle test calls
`tester.ensureSemantics()` since the semantics tree isn't built by
default in widget tests.

`flutter test` 71 → 74, all green; `flutter analyze` clean.

Also fixed two pre-existing `<name>` MD033 markdownlint warnings on
this task body — wrapped the placeholders in backticks.
