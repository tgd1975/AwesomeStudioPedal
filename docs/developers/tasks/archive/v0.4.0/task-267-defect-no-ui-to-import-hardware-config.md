---
id: TASK-267
title: Defect — Upload Hardware Config button is unreachable; the app has no UI to load a hardware config from disk
status: closed
closed: 2026-04-26
opened: 2026-04-26
effort: Small (2-4h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Low
human-in-loop: Main
epic: feature_test
order: 38
---

## Description

Discovered while running TASK-156 UP-08 on 2026-04-26 (Pixel 9, Android 16).

The Upload screen's **Upload Hardware Config** button
([app/lib/screens/upload_screen.dart:198-208](app/lib/screens/upload_screen.dart#L198-L208))
guards on `state.hardwareConfig` and shows
"No hardware config loaded." when it is `null`
([upload_screen.dart:81-84](app/lib/screens/upload_screen.dart#L81-L84)).

`ProfilesState` exposes a setter
[`loadHardwareConfigFromJson`](app/lib/models/profiles_state.dart#L23):

```dart
void loadHardwareConfigFromJson(Map<String, dynamic> json) {
  _hardwareConfig = HardwareConfig.fromJson(json);
  // ...
}
```

…but **nothing in the app calls it.** A grep across `app/lib`:

```
$ grep -rn "loadHardwareConfigFromJson\|setHardwareConfig" app/lib --include="*.dart"
app/lib/models/profiles_state.dart:23:  void loadHardwareConfigFromJson(...)
```

— a single hit (the definition itself). Profile List → Import JSON
only handles profiles. There is no Settings screen, no Import-Config
button, no auto-load from `assets/`. Consequence: on a fresh install
the **Upload Hardware Config** button can never succeed; tapping it
always shows "Upload Failed: No hardware config loaded." This makes
TASK-156 UP-08 unrunnable end-to-end without touching code, and means
that any user who wants to switch their pedal between ESP32 and
nRF52840 hardware mappings has no path to do so via the app.

## Reproducer

1. Fresh app install (or `pm clear com.example.awesome_studio_pedal`).
2. Connect to pedal, import any profiles JSON.
3. Open Upload screen → tap **Upload Hardware Config**.
4. Observe: error dialog "Upload Failed: No hardware config loaded."
5. There is no Profile List menu, Settings menu, or any other surface
   that imports a `config.json`.

## Acceptance Criteria

Pick the smaller of the two designs (A is simpler; B is the
long-term right answer):

**Option A (minimum to unblock UP-08):** add an "Import Config JSON"
action to the Profile List app-bar overflow (or the Upload screen
itself), mirroring the existing Import JSON flow. Calls
`loadHardwareConfigFromJson` with the picked file's parsed contents
and shows a "Hardware config loaded" SnackBar. Persisted via the
existing autosave mechanism so it survives app restarts.

**Option B (preferred):** auto-load `data/config.json` from bundled
`assets/` on first launch (the same fixture the firmware ships with),
so the button works out of the box. Then add the Import flow on top
for users who want to override.

Either way:

- [x] **Upload Hardware Config** button on a fresh install, after
      connecting, performs an actual upload (which exercises the same
      chunk path as TASK-261 — fix that first or in parallel).
- [x] On success the SnackBar "Hardware config uploaded!" appears and
      the pedal reboots.
- [x] Hardware identity check still works
      ([upload_screen.dart:88-100](app/lib/screens/upload_screen.dart#L88-L100))
      — uploading a `nrf52840` config to an `esp32` device aborts
      with the existing "Hardware mismatch" dialog (regression-test
      this with a hand-edited config).

## Test Plan

**Host (Flutter widget) test:** with whichever design lands, simulate
the import and assert `ProfilesState.hardwareConfig` is non-null after
the action completes.

**Manual on-device:** re-run TASK-156 UP-08 on Pixel 9 — the button
performs an upload and the pedal reboots with the new mapping.

## Notes

- This is a UX gap, not a code bug — the underlying state model and
  upload path are fine. The button has just been wired to a state
  field nothing populates.
- Out of scope but worth noting: the Profile List Import already does
  schema validation against `assets/profiles.schema.json`. The new
  Config Import should do the same against
  `assets/config.schema.json` (already shipped — see
  [app/lib/services/schema_service.dart:23](app/lib/services/schema_service.dart#L23)).
- Surfaced in TASK-156 UP-08 round-2 on 2026-04-26.
- Implementation: chose Option A (Import Hardware Config menu item on
  ProfileListScreen → routes through FileService.importHardwareConfig
  → schema-validates → loadHardwareConfigFromJson → autosaved to
  `config_autosave.json` and restored on launch alongside profiles).
- Side-fix during implementation: `app/assets/config.schema.json` was
  stale — missing the `hardware` and `pairing_pin` fields that the
  canonical `data/config.schema.json` (and `HardwareConfig.fromJson`)
  produce. Every real config.json was rejected with "unallowed
  additional property hardware" until the bundled schema was synced.
  Synced inline because UP-08 verification depended on it.
- Out of scope (filed as observation, not addressed): the bundled
  `app/assets/profiles.schema.json` is also stale relative to
  `data/profiles.schema.json` (missing `_meta`). It does not surface
  in the import-config flow but should be synced before the next
  TASK-156 PL-* round.
- Verified on-device on 2026-04-26 via `/verify-on-device` on Pixel 9 /
  Android 16:
  - Happy path: Profile List → More actions → Import Hardware Config
    → pick `asp_config.json` → "Hardware config loaded" SnackBar →
    navigate to Upload → tap **Upload Hardware Config** → "Hardware
    config uploaded!" SnackBar in 1 s.
  - Mismatch path: import a hand-edited `asp_config_nrf52840.json` →
    tap **Upload Hardware Config** on the connected esp32 device →
    "Upload Failed: Hardware mismatch: config targets \"nrf52840\"
    but connected device is \"esp32\". Upload aborted." dialog as
    expected.
  - Screenshots: `/tmp/verify_TASK-267_UP-08_result.png`,
    `/tmp/verify_TASK-267_UP-08_mismatch.png`.
