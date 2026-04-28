---
id: TASK-257
title: Defect — Action Editor "Key (raw HID)" values like 0x28 are not parsed by firmware
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (1-3h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: Main
epic: feature_test
order: 4
---

## Description

Discovered while resolving TASK-252. The Action Editor's "Key (raw HID)"
option lets the user enter a hex HID code such as `0x28` (KEY_RETURN) and
saves it as `SendKeyAction` with the literal string value `"0x28"`. The
JSON is well-formed and passes schema validation, but the firmware-side
parser in `lib/PedalLogic/src/config_loader.cpp` (`createActionFromJson`,
`SendKey` case at lines 275–283) calls `lookupKey()` to translate the
value to a `uint8_t` HID code:

```cpp
case Action::Type::SendKey:
{
    uint8_t code = lookupKey(actionJson["value"] | "");
    if (code != 0)
    {
        return std::make_unique<SendKeyAction>(keyboard, code);
    }
    break;  // unknown value -> nullptr -> action silently dropped
}
```

`lookupKey()` (`lib/PedalLogic/src/key_lookup.cpp:190`) is a `strcmp`
walk over a table of *named* keys (`KEY_RETURN`, `KEY_PAGE_UP`, …). For
the literal string `"0x28"` it returns 0, the action is dropped, and the
button does nothing on the device — no error surfaced anywhere.

So today the editor's "Key (raw HID)" path produces JSON the firmware
cannot consume. End-to-end the option is broken.

## Reproducer

1. In the app, edit any button via the Action Editor.
2. Choose Action Type → "Key (raw HID)".
3. Enter value `0x28` (KEY_RETURN).
4. Save, then upload profiles to the pedal.
5. Press the button on the pedal.
6. Observe: nothing happens. (Compare with "Key (named)" + `KEY_RETURN`,
   which works.)

## Acceptance Criteria

- [ ] `lookupKey()` (or the `SendKey` parser) accepts hex string values
      such as `"0x28"`, `"0X28"`, and decimal `"40"`, returning the
      same `uint8_t` HID code as the equivalent named lookup.
- [ ] `createActionFromJson` for `SendKey` falls back from
      `lookupKey()` to numeric parsing before giving up — the order of
      attempts is documented in a comment if non-obvious.
- [ ] Unknown / unparseable values still drop the action *and* log a
      clear error via `logger_->log(...)` so the failure is no longer
      silent.
- [ ] Re-run AE-07 on a real device with `0x28` and observe a
      KEY_RETURN keystroke.

## Test Plan

**Host tests** (`make test-host`):

- Add `test/unit/test_send_key_hex_value.cpp` (or extend
  `test_config_loader.cpp`).
- Cover:
  - `SendKeyAction` parses a hex value `"0x28"` to the same code as
    `"KEY_RETURN"`.
  - Uppercase `"0X28"` works identically.
  - Decimal `"40"` works identically.
  - Out-of-range / non-numeric / empty → action dropped *and* an error
    is logged.

**On-device tests**: not required — the parser logic is fully shimmable.
The AE-07 manual re-test in TASK-155 will cover the hardware path.

## Notes

- This blocks the AE-07 verification in TASK-155 from being meaningful.
  Add TASK-257 to TASK-155's `prerequisites:` when this is merged so
  the resume picks it up.
- Out of scope: revisiting whether "Key (named)" and "Key (raw HID)"
  should be merged into a single dropdown option (the editor already
  treats them as variants of the same `SendKeyAction` type — see
  TASK-252 resolution). Keep them split until UX feedback says
  otherwise.
- The silent-drop behaviour applies more broadly than just `SendKey` —
  several arms of `createActionFromJson` `break` without logging. A
  separate cleanup pass on logging is out of scope here.
