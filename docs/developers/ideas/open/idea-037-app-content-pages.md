---
id: IDEA-037
title: App content pages — Info/About, How-To, and supporting screens
category: apps
description: Add minimal in-app content pages — Info/About, a crisp How-To, and similar supporting screens — so the app is self-explanatory without external docs.
---

# App content pages — Info/About, How-To, and supporting screens

The mobile app today is essentially functional-only: scan, connect,
configure. There is no in-app explanation of *what* this project is,
*who* it is for, or *how* to actually use the pedal once paired. New
users (and curious GitHub visitors who install the app to take a look)
land directly in the configurator with no orientation.

This idea collects the content pages the app should grow, kept
deliberately short and crisp — "reduced to the max", not a manual
ported into the app.

## Candidate pages

- **Info / About**
  - One-paragraph project pitch (what the pedal is, why it exists).
  - Version, build, firmware compatibility range.
  - **Project links** (tappable, open in browser):
    - GitHub repository (source code, issues).
    - Project website / docs site (once it exists).
    - Release notes / changelog.
    - Report a bug → direct link to GitHub issues.
  - License and credits.
- **How-To (quickstart)**
  - 3–5 steps max, each one line: power on → pair → pick profile → press button.
  - One screenshot or icon per step, no walls of text.
- **Profiles — what they are**
  - Tiny explainer: a profile = a mapping from buttons to actions.
  - Pointer to the configurator, not a full reference.
- **Troubleshooting (very short)**
  - "Pedal not found?" → check power, check Bluetooth, check distance.
  - "Buttons do nothing?" → check active profile.
  - Each entry one line + one suggested action.
- **Legal / Open source**
  - License of the app and firmware.
  - Third-party licenses (auto-generated where possible).

## Startup splash screen

A short splash on app launch — primarily a branding moment, secondarily a
cover for the unavoidable startup work (Flutter engine warm-up, BLE adapter
init, last-known-pedal lookup) so the user sees something intentional
instead of a blank frame.

What it should show:

- The project mark ([asp-mark.svg](../../../design/handoff/logo/asp-mark.svg))
  centered, on the app's neutral background.
- Wordmark "AwesomeStudioPedal" underneath, same typographic treatment as
  the rest of the app.
- A thin progress indicator or pulsing accent — only if startup actually
  takes long enough to perceive. If init finishes in <200 ms, skip the
  indicator and just hold the logo for a minimum dwell.
- Version string in a muted footer (`v1.4.0` style) — same string the
  Info/About page shows, sourced once.

Behavior rules:

- **Minimum dwell** ~600 ms so the splash doesn't flicker on fast devices,
  **maximum dwell** ~2 s before we hand control to the next screen
  regardless of init state — long startup is a bug, not a UX feature.
- **Once per cold launch only.** Returning from background must not
  re-show the splash.
- Honors the system theme (light/dark) — no white flash on dark devices.
- Honors `MediaQuery.disableAnimations` / "reduce motion" — drop any
  pulse/fade and just hold the static logo.
- On Android, the platform splash (`android:windowSplashScreen…`) should
  visually match this Flutter splash so the handoff is seamless. Same on
  iOS via `LaunchScreen.storyboard`.

Where it should *not* go:

- Not as a marketing surface — no taglines, no "Welcome to…" copy.
- Not as a first-run tutorial host — that belongs in the How-To page or
  a one-time onboarding flow, both of which run *after* the splash.

Implementation note: Flutter's [`flutter_native_splash`](https://pub.dev/packages/flutter_native_splash)
package handles the native-side splash from a single config block in
`pubspec.yaml`, which keeps the Android/iOS launch screens in sync with
the in-app splash without hand-authored platform assets.

## Connection visibility

The user should always be able to see *what the app is currently
talking to* — not just an abstract "connected" badge.

- Show the connected pedal's identity somewhere persistent (e.g. a
  small status strip under the app bar): device name, maybe a short
  ID/MAC suffix, and connection state (scanning / connected /
  disconnected).
- Tapping the strip opens a small "Connection details" sheet with:
  - Device name and full identifier.
  - Firmware version reported by the pedal.
  - Signal strength (RSSI) if cheap to obtain.
  - Disconnect / forget actions.
- Same info should be reachable from the Info/About page so users can
  copy it into a bug report.

## "Connected pedal" page — board, firmware, current config

A dedicated read-only page that answers "what is on the pedal right
now?" — useful for debugging, sharing setups, and confirming an
upload landed.

What it should show:

- **Board** — identity string from the HW Identity characteristic
  (`esp32` / `nrf52840`), already exposed today via `kHwIdentityUuid`.
- **Firmware** — version, build date, git hash. *Not exposed by the
  firmware today.*
- **Current configuration** — the JSON the pedal is actually running
  with: profiles list, button-to-action mapping per profile, active
  profile index. *Not readable over BLE today — the config
  characteristic is write-only.*
- **Storage** — bytes used / `MAX_CONFIG_BYTES` ceiling.

## BLE / firmware changes this would require

Yes — the connection page mostly works with what we have, but the
"current configuration" and "firmware" parts need new surfaces.
Rough scope:

- **New read characteristic: firmware version** (e.g.
  `516515c5-…`). Returns a short string like
  `"1.4.0+build42 abcdef0"`. Cheap, read-only, no security
  implications.
- **Config readback** — two options, pick one:
  1. *Make the existing config characteristic readable* and return
     the canonical JSON the firmware is currently running with.
     Simple, but means storing/serialising the JSON on the device
     (memory cost on nRF52840 worth checking).
  2. *Add a separate "current config" read characteristic* that
     streams the active config back via the same chunked
     reassembler protocol used for uploads, just inverted.
- **Active profile index** — already in the config, but a tiny
  dedicated read/notify characteristic would let the app reflect
  profile switches in real time without re-reading the whole config.
- **Standard GATT services worth considering** for free metadata:
  - Device Information Service (0x180A) — manufacturer name, model
    number, firmware revision string. Standardised, picked up by
    generic BLE explorers too.
  - Battery Service (0x180F) — only relevant once we have a
    battery-powered variant (IDEA-012).

None of this is required for the *content pages* themselves
(Info/About, How-To, etc.) — those can ship purely client-side.
The Connected-Pedal page is the part that pulls in firmware work,
and is worth splitting into its own follow-up task once this idea
is realized, so the content pages don't get blocked on firmware
changes.

## "Live keystroke" page — show what the pedal just sent

A diagnostic page that mirrors, in real time, the key events the
pedal has emitted. Useful for:

- Confirming a freshly uploaded profile actually does what the user
  intended ("did button 3 really send `Ctrl+Shift+S` or just `S`?").
- Demoing the pedal to someone without needing a host app open.
- Bug reports — copy the rendered sequence into an issue.

### Display model

A scrolling tape of "key chips" newest-on-the-right (or top, TBD).
Each chip represents one key event (or one modifier+key combo if we
collapse them). Three rendering tiers, applied in order:

1. **Printable character** — render the literal glyph. `a`, `A`,
   `7`, `?`. Trivial branch.
2. **Named special key** — render an **icon + short label** from a
   lookup table (see below). `⏎ Enter`, `⇧ Shift`, `⏏ Eject`,
   `🔊 Vol+`. Label is shown for accessibility / when icon alone is
   ambiguous; small mode shows icon-only.
3. **Unknown / unmapped HID usage** — fall back to a monospace hex
   chip like `0x1A2`. Never crash, never hide the event.

Modifier + key combos collapse into a single chip joined by `+`:
`Ctrl + Shift + S`, modifier icons inline. A standalone modifier
press (no following key within N ms) renders on its own.

### The lookup table — single source of truth

The scalable part is **one declarative table** mapping HID usage ID
→ display descriptor, owned by the app (and ideally generated
rather than hand-typed):

```dart
// Sketch — actual shape TBD
class KeyDisplay {
  final String label;        // "Page Up", "Volume Up"
  final String? iconName;    // Material Symbols name, or null
  final String? glyph;       // "⏎", "⇧" — for icon-less terminals
  final KeyCategory category; // navigation, media, modifier, fn, system, …
}

const Map<int, KeyDisplay> kHidUsageDisplay = {
  0x28: KeyDisplay(label: 'Enter', iconName: 'keyboard_return', glyph: '⏎', …),
  0x4B: KeyDisplay(label: 'Page Up', iconName: 'keyboard_arrow_up', …),
  0xE9: KeyDisplay(label: 'Volume Up', iconName: 'volume_up', …),
  // …
};
```

Why this shape scales:

- **One table, many renderers.** Same data drives the live
  keystroke page, the profile editor's button-action picker, the
  "current config" readback display, and any future "preview what
  this profile does" view. Adding a key once lights it up
  everywhere.
- **Generated, not hand-curated.** The HID Usage Tables (Section
  10, Keyboard/Keypad Page 0x07; Section 15, Consumer Page 0x0C)
  are stable, public, and machine-readable. A small build-time
  script can ingest the spec and emit the Dart map, with a
  hand-maintained overlay for icons/glyphs the spec doesn't carry.
- **Categorised.** `KeyCategory` lets the UI group/colour chips
  (modifiers in grey, media in blue, fn keys in muted) and lets the
  profile editor present "Pick a media key" as a filtered picker.
- **Localizable.** The `label` is the only translated string; icons
  and glyphs are language-neutral.

### Rendering rules

- Icon-first when an icon exists and the chip is small; icon + label
  when there's room. Never label-only for keys that have a
  well-known glyph (`⇧`, `⏎`) — the glyph is faster to recognise.
- Repeated identical events collapse with a `×N` counter rather than
  filling the tape with 40 identical `Enter` chips.
- A "clear" and "copy as text" action on the tape — the latter
  emits a plain-text form (`Ctrl+Shift+S, Enter, Vol+ ×3`) suitable
  for pasting into a bug report.

### Where the events come from

This page only works if the firmware exposes what it just sent. Two
options, in increasing cost:

1. **Notify-on-send characteristic** — every time the pedal
   transmits an HID report, also notify a small BLE characteristic
   with `(usage_page, usage_id, modifiers, pressed/released)`. Cheap,
   real-time, but adds BLE traffic during heavy use.
2. **Loopback / preview mode** — pedal enters a mode where button
   presses *only* notify the app and do **not** emit HID. Useful
   for configuring without the host computer reacting. Worth
   pairing with the live page as a "Test mode" toggle.

Both should be considered alongside the firmware-readback work
above; they share the "expose more state over BLE" theme and can
land as a single follow-up task.

## Design constraints

- Crisp and minimal — every page should fit on one phone screen
  without scrolling where possible.
- No marketing copy. Plain, factual, helpful.
- Content lives in the repo (Markdown or structured strings) so it
  versions with the code and is easy to translate later.
- Reachable from a single entry point (e.g. a top-bar "i" / overflow
  menu) — do not clutter the main configurator UI.

## Open questions

- Source of truth: hand-written per page, or pull from existing
  developer docs (`docs/users/...`) so we don't maintain two copies?
- Localization: English only for now, or set up i18n scaffolding from
  day one?
- Should the How-To be static, or context-aware (e.g. show "pair your
  pedal" only while no pedal is connected)?
- Is there a "first run" flow that auto-shows the How-To once, or is
  everything purely opt-in via the menu?
