---
id: TASK-279
title: Defect — adding 8th profile silently rejected (no validation banner)
status: closed
closed: 2026-04-27
opened: 2026-04-27
effort: Small (<2h)
effort_actual: Small (<2h)
complexity: Junior
human-in-loop: Verify
epic: feature_test
order: 45
---

## Description

Surfaced while running TASK-157 R-07 on 2026-04-27 (Pixel 9, Android 16,
verified via `/verify-on-device` automation).

The pedal hardware can only address 7 profiles (3-bit LED selector). The
schema enforces this with `maxItems: 7` on the `profiles` array. The
ProfileList screen exposes an "Add profile" floating action button (FAB);
when the user is already at 7 profiles, the FAB **stays enabled**, the
"New Profile" dialog **still opens**, and the user can type a name and tap
"Add" — at which point the request is **silently dropped**: the dialog
closes, the validation banner remains green ("Valid ✓"), no SnackBar is
shown, and the new profile simply does not appear in the list.

Expected behaviour per FEATURE_TEST_PLAN.md Part 3, R-07: "Schema
validation error shown (max 7 for 3-bit LED)". Actual: no error feedback
at all.

This is a low-severity but high-confusion UX defect — a user who is
trying to grow their profile collection past 7 has no way to know why
their new profile is not appearing.

## Reproducer

1. Open the app, ensure 7 profiles are loaded (e.g. import
   `data/profiles.json` from this repo)
2. On the Profile List screen, tap the "Add profile" FAB
3. In the "New Profile" dialog, type any name (e.g. "08_Test")
4. Tap **Add**

Observed: dialog closes, list still shows 7 profiles, no error message.
Validation banner still reads "Valid ✓".

Expected: either (a) the FAB is disabled at 7 profiles with an
explanatory tooltip, or (b) tapping Add at 7 profiles surfaces a clear
error message ("Maximum 7 profiles — the pedal has a 3-bit selector and
cannot address more").

Screenshot: `/tmp/verify_TASK-157_r07_no_8th.png`.

## Acceptance Criteria

- [x] At 7 profiles, attempting to add an 8th surfaces a visible error
      to the user — FAB is disabled with a `Maximum 7 profiles reached`
      tooltip; SnackBar with same wording is the safety net for any
      programmatic call.
- [x] The error message names the limit (7). Hardware reason
      (LED selector) deliberately omitted from the user-facing message
      to keep it concise — the constraint lives in
      `app/lib/constants/profile_constants.dart` with the explanation.
- [x] R-07 in FEATURE_TEST_PLAN.md passes when re-run on a real device
      — verified 2026-04-27 on Pixel 9 / Android 16. FAB disabled at 7,
      flips back to enabled when count drops to 6, locks again when a
      7th profile is added. Screenshots:
      `/tmp/verify_TASK-279_fab_disabled_at_7.png`,
      `/tmp/verify_TASK-279_fab_enabled_at_6.png`.
- [x] In-app widget tests cover the cap behaviour:
      `app/test/widget/profile_list_new_profile_dialog_test.dart`
      adds three new tests (FAB enabled below cap; FAB disabled at cap
      with limit named in tooltip; FAB re-enables after a profile is
      removed). CLI-side cap also covered by
      `scripts/tests/test_pedal_config.py`'s
      `test_profiles_json_more_than_seven_returns_1`.

## Test Plan

**Host tests** — Flutter widget test for the ProfileList screen:

- Pump a ProfileList with 7 profiles fixture.
- Tap the "Add profile" FAB, fill in name, tap "Add".
- Assert either: FAB is disabled, OR a SnackBar with the limit message
  is shown, OR an error dialog/banner becomes visible.
- File location: `app/test/widget/profile_list_max_profiles_test.dart`
  (or extend an existing ProfileList widget test).

**On-device verification** — re-run TASK-157 R-07 manually after the
fix lands, confirm a clear error is shown and the profile count stays
at 7.

## Notes

- The schema layer (`SchemaValidator`) is presumably already aware of
  the 7-profile cap (since the `Valid ✓` / red banner system reflects
  schema validation). The issue is that the ProfileList "Add profile"
  flow likely calls a creation method that silently no-ops on overflow
  rather than throwing/returning a result the UI can surface.
- The fix may be as small as: (a) check `profiles.length >= 7` in the
  Add-profile handler and show a SnackBar before opening the dialog, or
  (b) disable the FAB when `profiles.length >= 7` and show an
  explanatory tooltip on long-press.
- Sibling defect tasks from the same TASK-157 run (if any) belong to
  the same `feature_test` epic.
