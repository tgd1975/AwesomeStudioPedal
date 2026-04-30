---
id: TASK-338
title: Add HID-usage display lookup table (generated) and Live-keystroke page
status: open
opened: 2026-04-30
effort: Large (8-24h)
complexity: Senior
human-in-loop: Clarification
epic: app-content-pages
order: 9
prerequisites: [TASK-337]
---

## Description

Two coupled deliverables — the lookup table is only useful with at
least one consumer, and the live-keystroke page is the most
illustrative consumer:

### A. Generated HID usage display table

Single source of truth mapping HID usage ID → display descriptor,
owned by the app and **generated** rather than hand-typed:

```dart
class KeyDisplay {
  final String label;        // "Page Up", "Volume Up"
  final String? iconName;    // Material Symbols name
  final String? glyph;       // "⏎", "⇧"
  final KeyCategory category;
}

const Map<int, KeyDisplay> kHidUsageDisplay = { /* generated */ };
```

A small build-time script ingests the public HID Usage Tables
(Section 10, Keyboard/Keypad Page 0x07; Section 15, Consumer Page
0x0C) and emits the Dart map, with a hand-maintained overlay for
icons/glyphs the spec doesn't carry. `KeyCategory` lets future UIs
group/colour and filter.

Why this shape — same table will eventually drive: the Live-keystroke
page, the profile editor's button-action picker, the Connected-Pedal
"current config" readback display, and any future "preview what this
profile does" view.

### B. Live-keystroke diagnostic page

Scrolling tape of "key chips" newest-first. Each chip applies the
three rendering tiers from IDEA-037: printable glyph → named-special
icon+label from the table → unmapped-HID hex fallback. Modifier+key
combos collapse into one chip (`Ctrl + Shift + S`). Repeated events
collapse with a `×N` counter. "Clear" and "Copy as text" actions on
the tape.

Event source — extend TASK-337's BLE work with **either**:

- (1) Notify-on-send characteristic — every HID report also notifies a
      small characteristic with `(usage_page, usage_id, modifiers,
      pressed/released)`. Adds BLE traffic during heavy use.
- (2) Loopback / preview mode — pedal enters a mode where button
      presses *only* notify the app and do not emit HID. Useful for
      configuration without the host computer reacting; pair with a
      "Test mode" toggle on this page.

Pick one (or both, if cheap) and document the choice in this task
before implementing. The firmware side of whichever option(s) win
land in TASK-337's scope or as a small extension here.

## Acceptance Criteria

- [ ] Build-time generator script lives under `scripts/` and re-runs
      idempotently (output diff = 0 on second run with no spec
      changes).
- [ ] Generated `kHidUsageDisplay` covers Keyboard/Keypad page (0x07)
      and Consumer page (0x0C) at minimum.
- [ ] Hand-maintained icon/glyph overlay is a separate file the
      generator merges in, not embedded in the generated output.
- [ ] Live-keystroke page renders the three rendering tiers correctly
      (test fixtures: `a`, `Enter`, an unmapped usage).
- [ ] Modifier+key collapse and repeated-event `×N` collapse both
      verified in tests.
- [ ] "Copy as text" emits the documented plain-text form
      (`Ctrl+Shift+S, Enter, Vol+ ×3`).

## Prerequisites

- **TASK-337** — provides the BLE firmware work this page depends on.
  The exact characteristic shape (notify-on-send vs loopback mode)
  is decided in this task and the firmware piece either rides along
  in TASK-337 or as a small extension at the start of this one.

## Test Plan

**Host tests** (Flutter widget + unit tests):

- `app/test/hid/hid_display_table_test.dart` — generated table is
  non-empty for pages 0x07 and 0x0C; overlay entries override
  generated defaults.
- `app/test/pages/live_keystroke_page_test.dart` — three rendering
  tiers; modifier+key collapse; repeated-event `×N`; copy-as-text
  format.

**On-device tests** (depending on which firmware option lands):

- If notify-on-send: extend the relevant `test/test_*_*/` to verify
  the new characteristic notifies on every emitted HID report.
- If loopback: verify HID emission is suppressed in loopback mode and
  the notify still fires.

## Documentation

- `docs/developers/BLE_PROTOCOL.md` — document the chosen event-
  source characteristic and any loopback-mode toggle.
- `docs/developers/ARCHITECTURE.md` — note the generated HID display
  table and its overlay contract.

## Notes

- Bundling the table with the live-keystroke page is intentional: the
  table without a consumer is dead weight, and the page without the
  table degenerates into a string of hex codes. Sized L because the
  generator + overlay design + UI + firmware decision together cross
  the M/L line; if the team prefers, split into:
  - 338a: generator + overlay
  - 338b: live-keystroke page
  - 338c: firmware event-source extension
  Decide at activation time.
- Localizable: `label` is the only translated string; icons/glyphs
  are language-neutral.
