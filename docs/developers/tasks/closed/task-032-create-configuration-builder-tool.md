---
id: TASK-032
title: Create configuration builder tool
status: closed
closed: 2026-04-17
opened: 2026-04-10
effort: Large (8-24h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Create a tool that helps users generate valid `pedal_config.json` / `profiles.json` files without
writing JSON by hand. Two implementation options are available — **Option A (web form) is recommended**
and is detailed in the implementation plan below.

## Acceptance Criteria

- [ ] Tool generates a valid `profiles.json` from user input
- [ ] Output validated against a JSON schema
- [ ] Tool linked from `README.md` and configuration documentation
- [ ] Usage instructions provided
- [ ] Hosted on GitHub Pages alongside the API docs (workflow updated)

## Implementation Plan

### Decision: Option A — Web Form

Host alongside the simulator (TASK-031) on GitHub Pages. Chosen over the Python CLI because:

- No installation required
- Can be linked directly from README
- Can share schema.json with the simulator's validation
- Lives alongside TASK-031 in the same Pages deployment

### File structure

```
docs/tools/config-builder/
  index.html    # Form UI
  builder.js    # JSON generation, validation, import/export logic
  schema.json   # JSON schema for profiles.json (authoritative)
```

### JSON Schema (`schema.json`)

The schema validates the `data/profiles.json` format exactly as the firmware parses it via
`ConfigLoader`. Key constraints:

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": ["profiles"],
  "properties": {
    "_doc": { "type": "string" },
    "profiles": {
      "type": "array",
      "minItems": 1,
      "maxItems": 7,
      "items": { "$ref": "#/definitions/profile" }
    }
  },
  "definitions": {
    "profile": {
      "type": "object",
      "required": ["name", "buttons"],
      "properties": {
        "name": { "type": "string" },
        "description": { "type": "string" },
        "buttons": {
          "type": "object",
          "patternProperties": {
            "^[A-Z]$": { "$ref": "#/definitions/action" }
          },
          "additionalProperties": false
        }
      }
    },
    "action": {
      "oneOf": [
        { "$ref": "#/definitions/sendCharAction" },
        { "$ref": "#/definitions/sendKeyAction" },
        { "$ref": "#/definitions/sendStringAction" },
        { "$ref": "#/definitions/sendMediaKeyAction" },
        { "$ref": "#/definitions/serialOutputAction" },
        { "$ref": "#/definitions/delayedAction" },
        { "$ref": "#/definitions/pinAction" }
      ]
    },
    "sendCharAction": {
      "type": "object",
      "required": ["type", "value"],
      "properties": {
        "type": { "const": "SendCharAction" },
        "name": { "type": "string" },
        "value": { "type": "string" }
      }
    },
    "sendKeyAction": {
      "type": "object",
      "required": ["type", "value"],
      "properties": {
        "type": { "const": "SendKeyAction" },
        "name": { "type": "string" },
        "value": { "type": "string" }
      }
    },
    "sendStringAction": {
      "type": "object",
      "required": ["type", "value"],
      "properties": {
        "type": { "const": "SendStringAction" },
        "name": { "type": "string" },
        "value": { "type": "string" }
      }
    },
    "sendMediaKeyAction": {
      "type": "object",
      "required": ["type", "value"],
      "properties": {
        "type": { "const": "SendMediaKeyAction" },
        "name": { "type": "string" },
        "value": { "type": "string", "enum": [
          "MEDIA_NEXT_TRACK", "KEY_MEDIA_NEXT_TRACK",
          "MEDIA_PREVIOUS_TRACK", "KEY_MEDIA_PREVIOUS_TRACK",
          "MEDIA_STOP", "KEY_MEDIA_STOP",
          "MEDIA_PLAY_PAUSE", "KEY_MEDIA_PLAY_PAUSE",
          "MEDIA_MUTE", "KEY_MEDIA_MUTE",
          "MEDIA_VOLUME_UP", "KEY_MEDIA_VOLUME_UP", "KEY_VOLUME_UP",
          "MEDIA_VOLUME_DOWN", "KEY_MEDIA_VOLUME_DOWN", "KEY_VOLUME_DOWN",
          "KEY_MEDIA_WWW_HOME", "KEY_MEDIA_WWW_BACK", "KEY_MEDIA_WWW_STOP",
          "KEY_MEDIA_WWW_SEARCH", "KEY_MEDIA_WWW_BOOKMARKS",
          "KEY_MEDIA_CALCULATOR", "KEY_MEDIA_EMAIL_READER",
          "KEY_MEDIA_LOCAL_MACHINE_BROWSER", "KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION"
        ]}
      }
    },
    "serialOutputAction": {
      "type": "object",
      "required": ["type", "value"],
      "properties": {
        "type": { "const": "SerialOutputAction" },
        "name": { "type": "string" },
        "value": { "type": "string" }
      }
    },
    "delayedAction": {
      "type": "object",
      "required": ["type", "delayMs", "action"],
      "properties": {
        "type": { "const": "DelayedAction" },
        "name": { "type": "string" },
        "delayMs": { "type": "integer", "minimum": 0 },
        "action": { "$ref": "#/definitions/action" }
      }
    },
    "pinAction": {
      "type": "object",
      "required": ["type", "pin"],
      "properties": {
        "type": { "enum": ["PinHighAction", "PinLowAction", "PinToggleAction",
                           "PinHighWhilePressedAction", "PinLowWhilePressedAction"] },
        "name": { "type": "string" },
        "pin": { "type": "integer", "minimum": 0, "maximum": 39 }
      }
    }
  }
}
```

### HTML form (`index.html`)

Structure:

```
<header>
  Title + link to KEY_REFERENCE.md

<main class="two-column">
  <section class="form-panel">
    <div id="profiles-tabs">         <!-- one tab per profile -->
      [Profile 1] [Profile 2] ... [+ Add Profile]
    </div>
    <div id="active-profile-form">
      <input name="profile-name">
      <textarea name="profile-description">
      <div class="buttons-grid">
        <!-- One row per button A, B, C, D -->
        Button A: [action-type dropdown] [conditional fields]
        Button B: ...
        Button C: ...
        Button D: ...
      </div>
      [Remove Profile]
    </div>

  <section class="preview-panel">
    <pre id="json-preview">   <!-- live JSON preview -->
    [Validate] [Download profiles.json] [Load existing file]
    <div id="validation-result">
```

Action type dropdown options (maps to `type` field):

| Label | JSON type |
|---|---|
| Key (named) | SendCharAction |
| Key (raw HID) | SendKeyAction |
| Type String | SendStringAction |
| Media Key | SendMediaKeyAction |
| Serial Output | SerialOutputAction |
| Delayed Action | DelayedAction |
| Pin High | PinHighAction |
| Pin Low | PinLowAction |
| Pin Toggle | PinToggleAction |
| Pin High While Pressed | PinHighWhilePressedAction |
| Pin Low While Pressed | PinLowWhilePressedAction |
| (none) | — omit button from output |

Conditional fields shown per type:

- **SendCharAction / SendKeyAction**: `value` text input + key-name autocomplete
  (autocomplete list from KEY_REFERENCE values: KEY_PAGE_UP, KEY_F1..F24, etc.)
- **SendStringAction / SerialOutputAction**: `value` textarea
- **SendMediaKeyAction**: `value` dropdown (enum from schema)
- **DelayedAction**: `delayMs` number input + nested action form (same dropdowns, 1 level deep)
- **Pin types**: `pin` number input (GPIO 0–39)
- All types: optional `name` text input

### JavaScript (`builder.js`)

Public API:

```javascript
// Called on any form change — rebuilds JSON and updates preview
function buildJson()              // → object matching profiles.json schema

// Runs ajv (bundled) against schema.json, returns {valid, errors}
function validateJson(json)

// Triggers <a download="profiles.json"> with JSON blob
function downloadJson()

// Reads a File object, parses JSON, calls populateForm()
function loadFromFile(file)

// Populates all form fields from a parsed profiles.json object
function populateForm(json)

// Adds a blank profile tab
function addProfile()

// Removes profile at index, re-renders tabs
function removeProfile(index)

// Renders conditional fields for the chosen action type
function renderActionFields(container, actionType, existingValues)
```

Validation library: use **[Ajv](https://ajv.js.org/)** v8 loaded from a CDN (`<script>` tag) — no
build step required. The schema.json is fetched at page load via `fetch('./schema.json')`.

Key code paths to get right:

- `buildJson()` must skip buttons where type is "(none)"
- `buildJson()` for `DelayedAction` must nest the inner action object correctly
- `populateForm()` must handle profiles.json files that have fewer than 4 buttons, or buttons
  labelled A–Z (not just A–D)
- `validateJson()` must display individual `ajv` error messages with a path (e.g.
  `profiles[1].buttons.A.value: must be equal to one of the allowed values`)

### GitHub Pages integration

The current `docs.yml` workflow deploys only `docs/api/html` (Doxygen output). The builder must
be included in the same Pages artifact.

Add a step **before** `actions/upload-pages-artifact` that copies the builder into the output dir:

```yaml
- name: Add tools to Pages artifact
  run: |
    mkdir -p docs/api/html/tools/config-builder
    cp -r docs/tools/config-builder/. docs/api/html/tools/config-builder/
```

After this change the builder is available at:
`https://tgd1975.github.io/AwesomeStudioPedal/tools/config-builder/`

### README and docs links

Add to `README.md` builder row:

```markdown
| Builder — I want to build one | ... · [Config Builder](https://tgd1975.github.io/AwesomeStudioPedal/tools/config-builder/) |
```

Add to `docs/builders/BUILD_GUIDE.md` (or a new `CONFIGURATION.md`) a section:

```markdown
## Configuration Builder

Use the web-based [Config Builder](https://tgd1975.github.io/AwesomeStudioPedal/tools/config-builder/)
to generate a `profiles.json` without writing JSON by hand.
```

## Test Plan

No automated tests required — this is a purely static web tool with no C++ logic.

Manual acceptance testing checklist:

- Load the page; add 2 profiles; assign one button per type (all 11 types); download → upload to
  device; verify pedal behaviour matches configuration
- Load an existing `data/profiles.json` via "Load existing file"; verify all fields populate
  correctly
- Delete a profile, re-add it; verify JSON preview updates instantly
- Enter an invalid config (e.g. `delayMs: -1`); verify validation shows an error with a path
- Open on mobile-sized viewport; verify form is usable

## Notes

- Ajv CDN: `https://cdn.jsdelivr.net/npm/ajv@8/dist/ajv.min.js` — verify this URL before use.
  Alternatively bundle Ajv as a local file in `docs/tools/config-builder/ajv.min.js` to avoid
  CDN dependency for offline use.
- Pin action types in `action.h` use enum values `PinHigh`, `PinLow`, `PinToggle`,
  `PinHighWhilePressed`, `PinLowWhilePressed` but the JSON `type` strings are defined in
  `config_loader.cpp` — verify exact strings before hardcoding in schema.
- The builder and simulator (TASK-031) share the same schema.json; keep it in one canonical
  location and load it via `fetch()` in both.
- TASK-031 and TASK-032 should be implemented together or TASK-032 first (simulator imports
  the schema).
- Related: TASK-031 simulator "Export Config" feature should call the same `downloadJson()` logic.
