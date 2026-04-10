---
id: TASK-032
title: Create configuration builder tool
status: open
opened: 2026-04-10
effort: Large (8-24h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Create a tool that helps users generate valid `pedal_config.json` files without writing JSON
by hand. Two implementation options are available — decide which fits best when picking up.

## Acceptance Criteria

- [ ] Tool generates a valid `pedal_config.json` from user input
- [ ] Output validated against a JSON schema
- [ ] Tool linked from `README.md` and configuration documentation
- [ ] Usage instructions provided

## Notes

**Option A (recommended): Web form** — hosted on GitHub Pages alongside the simulator (TASK-031).

```
docs/tools/config-builder/
  index.html    # Form with dropdowns for each button action
  builder.js    # Generates and validates JSON
  schema.json   # JSON schema for pedal_config.json
```

Form fields: profile name, Button A/B/C action (dropdown: Key, Media Key, String, None),
delay settings, LED feedback options. Output: downloadable `pedal_config.json`.

**Option B: Python CLI** — interactive script, simpler to implement.

```bash
python scripts/config_builder.py > my_config.json
```

JSON Schema excerpt:

```json
{
  "type": "object",
  "properties": {
    "profiles": {
      "type": "array",
      "items": {
        "properties": {
          "name": {"type": "string"},
          "buttons": {"patternProperties": {"^[A-C]$": {"$ref": "#/definitions/action"}}}
        }
      }
    }
  }
}
```

Related: TASK-031 (web simulator) could embed the builder as an "Export Config" feature.
