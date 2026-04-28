---
id: TASK-184
title: Schema defect — action value/pin fields not required
status: closed
closed: 2026-04-22
opened: 2026-04-21
effort: Small (<2h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: No
---

## Description

`profiles.schema.json` defines action types such as `SendCharAction`, `SendStringAction`,
`SendKeyAction`, `SendMediaKeyAction`, and `SerialOutputAction` as schema-valid with only a
`type` field present — `value` is not in the `required` array. Similarly, pin-based action
types (`PinHighAction`, `PinLowAction`, `PinToggleAction`, `PinHighWhilePressedAction`,
`PinLowWhilePressedAction`) do not require `pin`.

This means a profile containing `{"type": "SendCharAction"}` (no `value`) passes
`pedal_config.py validate` with exit 0 and can be uploaded to the pedal, where it will
fail silently or produce unexpected behaviour at runtime.

Discovered during TASK-149 feature test run (V-04).

## Acceptance Criteria

- [ ] Schema enforces `value` (string) as required for `SendCharAction`, `SendStringAction`,
  `SendKeyAction`, `SendMediaKeyAction`, `SerialOutputAction`
- [ ] Schema enforces `pin` (integer) as required for `PinHighAction`, `PinLowAction`,
  `PinToggleAction`, `PinHighWhilePressedAction`, `PinLowWhilePressedAction`
- [ ] `python scripts/pedal_config.py validate` exits 1 with a message mentioning `value`
  when given `{"profiles": [{"name": "X", "buttons": {"A": {"type": "SendCharAction"}}}]}`
- [ ] Existing valid `data/profiles.json` still validates with exit 0

## Test Plan

No automated C++ tests required — this is a schema-only change.

Manual verification after fix:

```bash
# Should exit 1 and mention 'value'
echo '{"profiles":[{"name":"X","buttons":{"A":{"type":"SendCharAction"}}}]}' > /tmp/bad.json
python scripts/pedal_config.py validate /tmp/bad.json

# Should exit 1 and mention 'pin'
echo '{"profiles":[{"name":"X","buttons":{"A":{"type":"PinHighAction"}}}]}' > /tmp/bad2.json
python scripts/pedal_config.py validate /tmp/bad2.json

# Should still exit 0
python scripts/pedal_config.py validate data/profiles.json
```

## Notes

The fix should use JSON Schema `if/then` or `oneOf` per action type to conditionally
require `value` or `pin` depending on the `type` field. Example pattern:

```json
"oneOf": [
  { "properties": { "type": { "const": "SendCharAction" } }, "required": ["type", "value"] },
  ...
]
```

The `actionObject` `$def` is shared by button slots, `longPress`, `doublePress`, `action`
(DelayedAction), and MacroAction steps — any change applies everywhere automatically.
