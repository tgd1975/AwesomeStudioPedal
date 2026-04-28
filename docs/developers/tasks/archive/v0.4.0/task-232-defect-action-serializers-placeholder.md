---
id: TASK-232
title: Defect — Action getJsonProperties placeholders lose information on round-trip
status: closed
opened: 2026-04-23
closed: 2026-04-24
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: No
epic: feature_test
order: 20
---

## Description

Several `Action` subclasses implement `getJsonProperties()` with placeholder
bodies that silently discard the action's actual payload. `SendCharAction` is
the known example found during TASK-151 U-02 execution:

```cpp
// lib/PedalLogic/include/send_action.h:73
void getJsonProperties(JsonObject& json) const override
{
    json["value"] = "CHAR"; // Simplified for now
}
```

Any pipeline that calls `getJsonProperties()` — currently including
`ConfigLoader::actionToJson` → `ConfigLoader::saveToFile` — corrupts the
round-tripped JSON. The on-disk `/profiles.json` ends up with every
`SendCharAction`'s value rewritten to the literal string `"CHAR"`, which then
fails to parse on the next boot. Error surfaces as:

```
SendChar: unknown key value: CHAR
...
Button A pressed (Profile: ABC)
  -> no action configured
```

The TASK-151 feature test surfaced this through the BLE upload path, where
`BleConfigReassembler::applyTransfer` was calling `saveToFile` to persist.
That path has now been patched (commit landing alongside this task) to write
the uploaded JSON **verbatim** instead of re-serialising from the in-memory
state, which sidesteps the placeholder but does not fix it.

### Placeholders surveyed

Audit all `Action` subclasses and confirm which have working vs. placeholder
serializers. Known placeholder: `SendCharAction`. Likely also affected (should
be verified):

- `SendKeyAction` — stores a keycode plus modifier, round-trip must preserve both.
- `SendMediaKeyAction` — stores a media-key enum.
- `SendStringAction` — stores a std::string; probably round-trips correctly.
- `SerialOutputAction` — stores a std::string; should round-trip.
- `MacroAction` — stores a vector of steps; round-trip is non-trivial.
- `DelayedAction` — stores a delay + a nested action; needs recursive
  serialisation.
- `PinAction` variants — `pin` is round-tripped in pin_action.cpp:36, but
  verify the `type` and any mode-specific fields.

## Acceptance Criteria

- [ ] Every `Action` subclass's `getJsonProperties` writes the full information
      needed to recreate the same action via `createActionFromJson` — no
      placeholders, no loss of the `value` / `code` / `delayMs` / `steps` fields.
- [ ] A host test loads `data/profiles.json`, serialises it back via
      `ConfigLoader::saveToFile` (to a buffer or temp file), parses the output
      again, and asserts the resulting `ProfileManager` state is identical to
      the first parse — for every action type we support.
- [ ] After the fix, `BleConfigReassembler::applyTransfer` could be reverted
      to use `saveToFile` without data loss; decide whether we want to or
      keep the verbatim-write approach (keeping verbatim is simpler; both
      should work after the fix).

## Test Plan

**Host tests** (`make test-host`):

- Add `test/unit/test_action_json_roundtrip.cpp` that builds a
  `ProfileManager` containing one action of each supported type with
  representative values, serialises via `ConfigLoader::saveToFile` to a
  buffer, reparses via `loadFromString`, and asserts the ProfileManager state
  is preserved.
- Register the new file in `test/CMakeLists.txt`.

## Notes

- The `// Simplified for now` comment dates to early scaffolding
  (TASK-102 `getJsonProperties on PinAction` was the intended follow-up, and
  only PinAction got done properly).
- The app's "read-back" feature (if we add one) and any "export-then-import"
  user flow depend on this being correct; not fixing this means any such
  feature also silently corrupts the configuration.
- The `#ifndef HOST_TEST_BUILD` guards around the placeholders are a smell —
  the host test build has no-op serialisers, so host tests never catch this
  class of bug. Move the guards or remove them once the implementations are
  platform-agnostic (ArduinoJson works on host — see PinAction for a host-safe
  example).
