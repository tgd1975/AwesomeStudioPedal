---
id: TASK-264
title: Defect — Profile List validation banner stays green when an action's value is schema-valid but runtime-unresolvable
status: closed
closed: 2026-04-27
opened: 2026-04-26
effort: Small (2-4h)
effort_actual: Medium (2-8h)
complexity: Medium
human-in-loop: Main
epic: feature_test
order: 35
---

## Description

Discovered while running TASK-154 PL-12 on 2026-04-26.

The Profile List shows a validation banner ("Valid ✓" green or
"N error(s)" red) above the list. It correctly turns red when:

- A profile's action `type` is not in the schema enum (e.g.
  `"NotARealAction"`).
- A profile is missing required fields (e.g. no `buttons`).

It does **not** turn red for action values that are syntactically
fine (string, schema-acceptable) but semantically invalid — i.e.
values the firmware will not be able to resolve at runtime.

**Concrete reproducer used in the test:** a profile with

```json
{"type": "SendCharAction", "name": "Prev Page", "value": "NOT_A_VALID_KEY_VALUE"}
```

The schema accepts `value` as any string; `lookupKey()` (firmware) will
return 0 and the action will be silently dropped on the device. The
app's validation banner stays green Valid ✓ — the user has no way
to learn at the list level that their button is broken until they
upload, look at the pedal, and notice nothing happens.

This is a related-but-distinct issue from TASK-257: TASK-257 fixed
the firmware to *accept hex/decimal values*; this task is about
making the app *flag* values that won't resolve on either side
(named-key tables in firmware, hex/decimal range, etc.).

## Reproducer

1. Take `data/profiles.json`. In the first profile, change Button A's
   `value` from `"KEY_PAGE_UP"` to `"NOT_A_VALID_KEY_VALUE"` (keep
   `type: "SendCharAction"`). Save as `profiles_invalid_action.json`.
2. Push to `/sdcard/Download/` on the test device.
3. In the app: Edit profiles → Import JSON → pick that file.
4. Observe: "Profiles imported" snackbar; banner stays **Valid ✓**
   (green). Expected: red `1 error(s)` banner pointing at
   `/profiles/0/buttons/A/value`.

## Acceptance Criteria

- [ ] The Profile List validator runs the same value-resolution
      logic the firmware uses (named-key table, hex/decimal range)
      for `SendCharAction`, `SendKeyAction`, and `SendMediaKeyAction`.
- [ ] When any profile has at least one action whose value cannot
      be resolved, the banner shows red `N error(s)`.
- [ ] Tapping the red banner (PL-13) lists the unresolvable values
      with their JSON-pointer paths (e.g.
      `/profiles/0/buttons/A/value: cannot resolve "NOT_A_VALID_KEY_VALUE"
      as a named key, hex code, or decimal code in [1, 255]`).
- [ ] The same check blocks Save in the Action Editor for the same
      input — see TASK-265 for the related editor crash that
      surfaces the absence of this check.

## Test Plan

**Host tests** (`make test-host`) where the validator is reachable
without the Flutter framework:

- If the validation logic lives in shared Dart code, this is a
  Flutter widget/unit test in `app/test/` (out of scope for the C++
  host suite).
- Either way, add a test that constructs a `Profiles` object with the
  reproducer above and asserts the validation result is `red, 1 error`
  with a message pointing at `/profiles/0/buttons/A/value`.

**Manual on-device:** re-run TASK-154 PL-12 with the
`profiles_invalid_action.json` fixture and observe the red banner
with the expected error.

## Notes

- The named-key table the app validator uses must stay in sync with
  the firmware's `lib/PedalLogic/src/key_lookup.cpp`. Single source
  of truth options: codegen the Dart enum from the C++ table, or
  expose the table via a generated JSON file consumed by both.
  This is out of scope for this task — a follow-up if the validator
  fix lands.
- TASK-257 fixed the firmware-side parser to *accept* hex/decimal
  values. The validator needs the matching acceptance logic, not
  just named-key lookup.
- Surfaced in TASK-154 PL-12 re-run on 2026-04-26.

## Resolution

Fixed in commit on `feature/idea-realizations` (2026-04-27):

- New service:
  [app/lib/services/action_value_resolver.dart](../../../../app/lib/services/action_value_resolver.dart)
  — mirrors the firmware's value-resolution rules
  (`lookupKey()` for `SendKeyAction` / legacy `SendCharAction`,
  hex/decimal in [1, 255], `kMediaKeyValues` for media keys,
  free-text for `SendStringAction` / `SerialOutputAction`). Includes
  `findUnresolvable(json)` which walks the profile tree and returns
  JSON-pointer-pathed error strings.
- [app/lib/services/schema_service.dart](../../../../app/lib/services/schema_service.dart):
  `validateProfiles` now runs the runtime check after the schema
  check. `ValidationResult` gained an `isSchemaError` flag so callers
  can distinguish "structurally invalid — refuse to load" from
  "runtime-unresolvable — load but flag in the banner".
- [app/lib/services/file_service.dart](../../../../app/lib/services/file_service.dart):
  `importProfiles` accepts profiles with runtime-resolvability
  errors (the user must be able to see and fix the bad values inside
  the app); only schema errors block the import.
- Action Editor Save path now calls `ActionValueResolver.resolve()`
  on the form state and refuses to save with a SnackBar
  ("Cannot save: …") if the value would not resolve at runtime.

Verified on-device on 2026-04-27 on Pixel 9 / Android 16:

1. Imported `profiles_invalid_action.json` (Button A = SendCharAction,
   value = `NOT_A_VALID_KEY_VALUE`). The profile loaded; the banner
   turned **red "1 error"** (was green Valid ✓ before the fix).
2. Tapped the red banner → "Validation Errors" dialog showed
   `/profiles/0/buttons/A/value: cannot resolve "NOT_A_VALID_KEY_VALUE"
   as a single character or named key` — JSON-pointer path matches
   the AC.
3. Action Editor opened on Button A without the dropdown crash (see
   TASK-265). Tapping Save with the bad value still in the field was
   blocked by the editor's resolvability check; replacing the value
   with `KEY_PAGE_UP` allowed Save and Button A round-tripped to
   `SendKeyAction / KEY_PAGE_UP / Prev Page` in JSON Preview.

Screenshots: `/tmp/verify_TASK-155_pl13_error_dialog.png`,
`/tmp/verify_TASK-155_ae_265_no_crash.png`,
`/tmp/verify_TASK-155_ae_save_blocked.png`.

**Follow-up filed:** **idea-045** — the Profile List banner only
revalidates when the profile count changes; after fixing a bad value
inside the app, the banner stays red until the user navigates away
and back. Pre-existing bug, made more visible by this fix; out of
scope here.
