---
id: TASK-277
title: Defect — "Key (raw HID)" value field hint reads `e.g. 0x28` but `0x28` does not type Enter
status: closed
closed: 2026-04-27
opened: 2026-04-27
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Low
human-in-loop: Verify
epic: feature_test
order: 43
---

## Description

Surfaced while running TASK-155 round 3 AE-07 on 2026-04-27 (Pixel 9,
Android 16, verified via `/verify-on-device TASK-155 AE-07`).

The Action Editor's "Key (raw HID)" value field shows the placeholder
hint `HID code (e.g. 0x28)`. `0x28` is the standard USB HID Usage ID
for the Enter key — but the firmware's parser does **not** treat the
field as a USB HID Usage ID. It uses the firmware-internal NicoHood
BleKeyboard scheme (see `lib/hardware/.../i_ble_keyboard.h`), in
which Enter is `0xB0`, Esc is `0xB1`, Tab is `0xB3`, etc.

Net effect: a user who types `0x28` (taking the hint at face value)
will see the action save successfully, upload to the pedal, press the
button, and **not** get Enter on the host. The hint is actively
misleading rather than ambiguous.

This was anticipated as a separate decision in **idea-039**
(USB-HID-Usage-ID vs firmware-internal codes) and called out in the
Notes section of [TASK-155](../active/task-155-feature-test-app-action-editor.md).
The hint text fix is decoupled from the broader idea-039 architecture
decision: regardless of whether the field eventually accepts USB HID
codes or stays on internal codes, the hint should not advertise a
value that produces the wrong key.

## Reproducer

1. Open the Action Editor for any button, set Action Type to
   "Key (raw HID)".
2. Read the placeholder hint in the value field: `HID code (e.g. 0x28)`.
3. Either: (a) take the hint at face value, enter `0x28`, save,
   upload, press the button — observe: nothing types Enter on the
   host; or (b) cross-reference TASK-155 Notes / idea-039 to learn
   that `0xB0` is the right firmware-internal code for Enter.

## Acceptance Criteria

- [ ] The "Key (raw HID)" hint either:
      - changes its example to a firmware-internal code that actually
        types the named key (e.g. `e.g. 0xB0 (Enter)`), and clarifies
        the field is "firmware-internal HID" not "USB HID Usage ID";
      OR
      - the field accepts USB HID Usage IDs and the parser/firmware
        layer is updated accordingly (this is the broader idea-039
        decision — out of scope for this task; see Notes).
- [ ] Whichever direction is taken, the result is consistent across
      the hint text, the field-level help/tooltip (if any), the
      app-side serializer, and the firmware-side parser.
- [ ] FEATURE_TEST_PLAN.md AE-07 reference to TASK-277 can be marked
      resolved.

## Test Plan

**Host tests** (`make test-host`):

- A small widget test that mounts the Action Editor with Action Type
  = "Key (raw HID)" and asserts the value field's hint matches the
  decided wording (avoids regressing back to `0x28` accidentally).

**On-device verification** via `/verify-on-device TASK-155 AE-07` —
the existing recipe already enters `0xB0` and asserts the JSON; the
hint-text fix doesn't change the JSON assertion, so re-running the
recipe is a smoke check rather than a verification of this specific
fix.

## Notes

- Surfaced in TASK-155 round 3, 2026-04-27. Screenshot:
  `/tmp/verify_TASK-155_ae_07_json_preview.png`.
- This task is the **hint-text** subset of idea-039 — narrowly
  scoped so it can ship independently of the larger
  USB-HID-Usage-ID-vs-firmware-internal-codes decision. If idea-039
  later flips the field's value space, revisit this task and the
  hint together.
- Cross-reference: [idea-039 in docs/developers/ideas/](../../ideas/)
  and the Notes section of
  [TASK-155](../active/task-155-feature-test-app-action-editor.md).
- The same misleading example may exist in any user-facing
  documentation that quotes `0x28` for Enter — search for it before
  closing.

## Resolution

Fixed in commit on `feature/idea-realizations` (2026-04-27):

- [app/lib/widgets/key_value_field.dart](../../../../app/lib/widgets/key_value_field.dart):
  the "Key (raw HID)" branch now uses a dedicated `TextField`
  with `labelText: 'Firmware HID code (e.g. 0xB0 = Enter)'` and a
  `helperText` reading "Firmware-internal codes, not USB HID Usage IDs
  (e.g. 0x28 ≠ Enter here).". A user who copies the example directly
  now gets Enter; the helperText prevents anyone from assuming the
  field accepts USB HID Usage IDs.
- [app/lib/constants/action_types.dart](../../../../app/lib/constants/action_types.dart):
  doc-comment example updated from `0x28` to `0xB0` for consistency.
- [app/test/widget/key_value_field_test.dart](../../../../app/test/widget/key_value_field_test.dart):
  the existing widget test for the raw-HID branch was inverted to
  guard against regression — it now asserts the label contains
  `0xB0` and the helperText contains `Firmware-internal`, and that
  no chrome contains the misleading `0x28)` leading example.

The broader idea-039 question (USB-HID-Usage-ID vs firmware-internal
codes as the field's value space) remains open and is decoupled from
this task. If idea-039 later flips the value space, revisit this
hint together with the parser.

Verified on-device on 2026-04-27 on Pixel 9 / Android 16:

- Action Editor → Action Type → "Key (raw HID)" — value field's
  label reads `Firmware HID code (e.g. 0xB0 = Enter)` and the
  helperText below reads `Firmware-internal codes, not USB HID
  Usage IDs (e.g. 0x28 ≠ Enter here)`. Screenshot:
  `/tmp/verify_TASK-155_task277_hint.png`.

Other `0x28` occurrences in the repo are either historical records
(FEATURE_TEST_PLAN.md round-2 row, this task's reproducer, idea-039
notes) and intentionally preserved, or in this task's resolution
itself as the *negative* example.
