---
id: TASK-155
title: Feature Test — App action editor (all action types)
status: closed
closed: 2026-04-27
opened: 2026-04-19
effort: Small (2-4h)
effort_actual: Medium (2-8h)
complexity: Low
human-in-loop: Main
epic: feature_test
order: 7
prerequisites: [TASK-251, TASK-252, TASK-253, TASK-257]
---

## Description

Execute the functional and usability tests for the Action Editor screen covering all supported
action types, as defined in `docs/developers/FEATURE_TEST_PLAN.md` Part 2.5.

All tests can be performed on the emulator; use JSON Preview to verify produced JSON structure
without needing a live pedal.

## Pre-conditions

- Flutter app running on test device or Lean_API33 emulator
- At least one profile with at least one button slot accessible

## Tests to execute

### Named Key & Media Key

| Test ID | Description |
|---------|-------------|
| AE-01 | Select "Key (named)" → value field with autocomplete appears |
| AE-02 | Type "KEY_F" → autocomplete shows KEY_F1, KEY_F2 … KEY_F12 etc. |
| AE-03 | Select KEY_PAGE_UP, display name "Next Page", Save → row shows "Next Page"; JSON correct |
| AE-04 | Select "Media Key" → autocomplete; type "PLAY" filters results |
| AE-05 | Select MEDIA_PLAY_PAUSE, Save → JSON shows `SendMediaKeyAction` / `MEDIA_PLAY_PAUSE` |

### String & Raw Key

| Test ID | Description |
|---------|-------------|
| AE-06 | Select "Type String", enter "ctrl+z" → JSON shows `SendStringAction` |
| AE-07 | Select "Key (raw HID)", enter "0xB0" (KEY_RETURN, firmware-internal code) → JSON shows `SendKeyAction` / `0xB0`; pressing the button on the pedal types Enter. See **Notes** for why `0x28` does not work despite being the standard USB HID Usage ID for Enter. |

### Pin Actions

| Test ID | Description |
|---------|-------------|
| AE-08 | Select "Pin High" → pin number field appears |
| AE-09 | Enter pin 27, Save → JSON shows `PinHighAction` / `27` |
| AE-10 | Enter non-numeric pin value → field shows error; Save blocked |
| AE-11 | Select "Pin High While Pressed", pin 14, Save → JSON shows `PinHighWhilePressedAction` |

### Serial Output

| Test ID | Description |
|---------|-------------|
| AE-12 | Select "Serial Output", enter "test message" → JSON shows `SerialOutputAction` |

### Save / Cancel

| Test ID | Description |
|---------|-------------|
| AE-13 | Tap Save with no action type selected → Save disabled or shows error |
| AE-14 | Tap Save with action type set → returns to Profile Editor; row updates |
| AE-15 | Navigate back without tapping Save → action unchanged |

### Usability

| Test ID | Description |
|---------|-------------|
| AE-U1 | Musician sets button A to "Play/Pause media" without guidance; target < 60 s |
| AE-U2 | Is "Key (named)" vs "Key (raw HID)" vs "Media Key" terminology clear to a non-developer? |

## Acceptance Criteria

- [x] All AE-01–AE-15 tests executed and results recorded in FEATURE_TEST_PLAN.md (round 3, 2026-04-27 — all ✓)
- [ ] AE-U1 timed and result noted; AE-U2 findings documented — **deferred to TASK-278** (needs non-developer tester; not blocking closure of this task)
- [x] Any failures filed as child tasks using the defect template (TASK-251/252/253/257 closed; TASK-276/277 + idea-044 filed in round 3)

## Round 3 (2026-04-27, automated re-run via verify-on-device)

After all four prerequisite defects (TASK-251/252/253/257) closed, the
six previously-failing or artifact-affected rows were re-run on the
connected Pixel 9 / Android 16 using the new AE-* recipes added to
[`/verify-on-device`](../../../../.claude/skills/verify-on-device/SKILL.md):

| Row | Round 2 | Round 3 | Confirms |
|-----|---------|---------|----------|
| AE-03 | ~ | ✓ | TASK-252 (`SendKeyAction` written, not `SendCharAction`) |
| AE-04 | ✗ | ✓ | TASK-253 (filterable EditText autocomplete; no overflow) |
| AE-06 | ✓† | ✓ | `+` literal preserved end-to-end (prior `~` was an adb-tooling artifact) |
| AE-07 | ✓† | ✓ (JSON leg) | TASK-257 (hex value saves as `SendKeyAction` / `0xB0`); hardware leg still deferred |
| AE-14 | ✗ | ✓ | TASK-252 (Save returns to Profile Editor) |
| AE-15 | ~ | ✓ | TASK-251 (BACK returns to Profile Editor; data unchanged) |

Three side observations spun out as new tasks/idea (filed before
closing this task):

- **TASK-276** — Action Editor silently drops the `longPress` field
  when a button's primary action is re-saved through the editor.
- **TASK-277** — "Key (raw HID)" value field hint reads `e.g. 0x28`
  but `0x28` does not type Enter under the firmware's internal-code
  scheme.
- **idea-044** — Value field doesn't reset when Action Type changes
  (cosmetic UX).

AE-U1 / AE-U2 usability checks remain deferred — they require a
non-developer tester. Tracked as **TASK-278**, which carries the
human-led portion of the AC forward independently of this task.

## Paused

- 2026-04-26: Paused mid-run. Defects filed and listed as `prerequisites:`:
  - TASK-251 — system BACK exits the app from any sub-screen; breaks AE-15
    (the navigation half — the "data unchanged" half passes). *(closed 2026-04-26)*
  - TASK-252 — Action Editor: Save persists but does not navigate (AE-14);
    "Key (named)" + KEY_PAGE_UP saves as `SendCharAction` instead of
    `SendKeyAction` (AE-03). *(closed 2026-04-26)*
  - TASK-253 — Media Key value field overflows by 79 px and the dropdown
    is read-only (no type-to-filter); breaks AE-04. *(closed 2026-04-26)*
  - TASK-257 — "Key (raw HID)" with hex value (e.g. `0x28`) saves as
    `SendKeyAction` JSON but the firmware parser rejects the hex string
    (only named-key strings match). Blocks AE-07 end-to-end on hardware.
    *(filed 2026-04-26 as a follow-up to TASK-252; active.)*
- Resume with `/ts-task-active TASK-155` once TASK-257 closes.

## Notes

### "Key (raw HID)" value space — firmware-internal codes, not USB HID Usage IDs

When TASK-257 was closed (2026-04-26), the firmware-side parser was extended to
accept hex/decimal SendKey values (e.g. `0xB0`, `176`). However a related
ambiguity surfaced that affects AE-07:

The hex value the user enters in the "Key (raw HID)" field is **not** the
standard USB HID Usage ID — it is the firmware's internal code (the NicoHood
BleKeyboard / `i_ble_keyboard.h` scheme).

| Key | Standard USB HID Usage ID | Firmware-internal code |
|-----|---------------------------|------------------------|
| Enter | `0x28` | `0xB0` |
| Esc | `0x29` | `0xB1` |
| Backspace | `0x2A` | `0xB2` |
| Tab | `0x2B` | `0xB3` |
| F1 | `0x3A` | `0xC2` |
| Right Arrow | `0x4F` | `0xD7` |

So entering `0x28` in the "Key (raw HID)" field will **not** type Enter on
the host — `0x28` is a valid HID-range value but maps to a different
firmware-internal code (or none at all). Whether this is "the firmware
mapping is wrong" or "the field is correctly labeled but the conventional
table everyone expects is the USB one" is an open UX/architecture question;
it is filed as **idea-039** for a separate decision.

For AE-07 in this task, use the firmware-internal code (e.g. `0xB0` for
KEY_RETURN). For the broader question, see idea-039.
