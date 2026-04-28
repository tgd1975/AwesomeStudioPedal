---
id: TASK-031
title: Create web-based simulator
status: closed
closed: 2026-04-17
opened: 2026-04-10
effort: Extra Large (24-40h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Create a browser-based pedal simulator hosted on GitHub Pages, allowing users to test button
mappings and preview BLE keyboard output without physical hardware.

## Acceptance Criteria

- [ ] Simulator hosted at GitHub Pages URL (already configured)
- [ ] Visual representation of pedal buttons
- [ ] Load a `profiles.json` file to populate button mappings
- [ ] Clicking a button shows the simulated BLE keyboard output
- [ ] Simulated serial output display
- [ ] Configuration can be saved/exported
- [ ] Linked from `README.md`

## Implementation Plan

### Prerequisite

**Complete TASK-032 first.** The simulator shares `schema.json` with the config builder for
validation, and the "Export Config" feature reuses the builder's `downloadJson()` logic. Implement
TASK-032 before or alongside this task.

### File structure

```
docs/simulator/
  index.html      # Main simulator page
  simulator.js    # Button logic, config parsing, output formatting
  styles.css      # Pedal visual, LEDs, output panels
  example.json    # The contents of data/profiles.json — shipped as a working example
```

### GitHub Pages integration

The `docs.yml` workflow currently deploys only `docs/api/html` (Doxygen). Add a step **before**
`actions/upload-pages-artifact` to copy both the simulator and the config builder into the output:

```yaml
- name: Add simulator and tools to Pages artifact
  run: |
    mkdir -p docs/api/html/simulator
    cp -r docs/simulator/. docs/api/html/simulator/
    mkdir -p docs/api/html/tools/config-builder
    cp -r docs/tools/config-builder/. docs/api/html/tools/config-builder/
```

Resulting URLs:

- Simulator: `https://tgd1975.github.io/AwesomeStudioPedal/simulator/`
- Config Builder: `https://tgd1975.github.io/AwesomeStudioPedal/tools/config-builder/`

### Visual layout (`index.html`)

The page has three regions:

```
┌──────────────────────────────────────────────────┐
│  HEADER: title, load-config button, profile nav  │
├───────────────────┬──────────────────────────────┤
│  PEDAL PANEL      │  OUTPUT PANEL                │
│                   │                              │
│  [BLE LED] [PWR]  │  ┌──────────────────────┐   │
│                   │  │ BLE Output           │   │
│  ┌───┐ ┌───┐      │  │ > KEY_PAGE_DOWN      │   │
│  │ A │ │ B │      │  │ > ctrl+z             │   │
│  └───┘ └───┘      │  └──────────────────────┘   │
│  ┌───┐ ┌───┐      │  ┌──────────────────────┐   │
│  │ C │ │ D │      │  │ Serial Output        │   │
│  └───┘ └───┘      │  │ > Pedal alive.       │   │
│                   │  └──────────────────────┘   │
│  [○] [○] [○]      │                              │
│  Profile LEDs     │  [Clear Output]  [Export]    │
│  [  SELECT  ]     │                              │
└───────────────────┴──────────────────────────────┘
```

HTML structure:

```html
<header>
  <h1>AwesomeStudioPedal Simulator</h1>
  <div class="toolbar">
    <label class="load-btn">Load profiles.json <input type="file" id="config-file" accept=".json"></label>
    <button id="load-example">Load Example</button>
    <span id="config-name">No config loaded</span>
  </div>
</header>

<main>
  <section class="pedal-panel">
    <div class="status-leds">
      <div class="led" id="ble-led" title="Bluetooth status"></div>
      <div class="led" id="pwr-led lit" title="Power"></div>
    </div>

    <div class="button-grid">
      <button class="pedal-btn" id="btn-A" data-btn="A">
        <span class="btn-label">A</span>
        <span class="btn-name"></span>   <!-- populated from config -->
      </button>
      <!-- B, C, D identical -->
    </div>

    <div class="profile-section">
      <div class="profile-leds">
        <div class="led" id="profile-led-0"></div>
        <div class="led" id="profile-led-1"></div>
        <div class="led" id="profile-led-2"></div>
      </div>
      <button id="select-btn">SELECT</button>
      <span id="profile-name"></span>
    </div>
  </section>

  <section class="output-panel">
    <div class="output-block">
      <h3>BLE Output</h3>
      <div id="ble-output" class="output-log"></div>
    </div>
    <div class="output-block">
      <h3>Serial Output</h3>
      <div id="serial-output" class="output-log"></div>
    </div>
    <div class="output-actions">
      <button id="clear-btn">Clear Output</button>
      <button id="export-btn">Export profiles.json</button>
    </div>
  </section>
</main>
```

### JavaScript (`simulator.js`)

#### State

```javascript
let config = null;          // parsed profiles.json object
let currentProfile = 0;     // index into config.profiles
let heldButtons = new Set(); // buttons currently held (for WhilePressed actions)
```

#### Public functions

```javascript
/**
 * Parse and load a profiles.json object. Validates via schema.json (shared with builder).
 * On success: populates button labels, resets to profile 0, clears output.
 * On failure: shows error in output panel, leaves previous config intact.
 */
function loadConfig(jsonData)

/**
 * Switch to profile at index. Wraps around (mod numProfiles).
 * Updates profile LEDs, profile name display, and all button labels.
 * Binary-encodes index into the 3 profile LEDs (same as hardware: LED0=bit0, LED1=bit1, LED2=bit2).
 */
function selectProfile(index)

/**
 * Called on mousedown / touchstart for button id ('A'–'Z').
 * Executes the action for currentProfile's button[id].
 * Appends formatted output to the appropriate output log.
 * For WhilePressed actions: marks button as held (starts "held" CSS class).
 */
function simulateButtonPress(id)

/**
 * Called on mouseup / touchend for button id.
 * For WhilePressed actions: clears held state, appends "released" message.
 * No-op for other action types.
 */
function simulateButtonRelease(id)

/**
 * Returns a human-readable string describing what the action sends.
 * Used to populate the output log.
 *
 * Examples:
 *   SendCharAction("KEY_PAGE_DOWN")  → "KEY: KEY_PAGE_DOWN"
 *   SendStringAction("ctrl+z")       → 'TYPE: "ctrl+z"'
 *   SendMediaKeyAction("MEDIA_STOP") → "MEDIA: MEDIA_STOP"
 *   SerialOutputAction("alive")      → "SERIAL: alive"
 *   DelayedAction(3000, inner)       → "DELAY 3000ms → " + formatActionOutput(inner)
 *   PinHighAction(pin=13)            → "PIN 13: HIGH"
 *   PinHighWhilePressedAction(13)    → "PIN 13: HIGH (held)"
 */
function formatActionOutput(action)

/**
 * Appends a timestamped line to the output log element.
 * Format: "[HH:MM:SS] <message>"
 */
function appendOutput(logElement, message)

/**
 * Triggers download of current config as profiles.json.
 * Delegates to builder.js downloadJson() if available; otherwise serialises config directly.
 */
function exportConfig()
```

#### DelayedAction behaviour

When a `DelayedAction` is pressed:

1. Append `"DELAY Xms → ..."` immediately to BLE output
2. Show a countdown indicator on the button (CSS animation or updating text)
3. After `delayMs` milliseconds (via `setTimeout`), append the inner action's output
4. If the user presses SELECT (changes profile) before the delay fires: cancel the timer
   (`clearTimeout`) and append `"DELAY CANCELLED"` to BLE output

#### Profile LED encoding

The hardware encodes the profile index in binary across 3 LEDs (LED0 = bit 0, LED1 = bit 1,
LED2 = bit 2). The simulator must match this exactly:

```javascript
function updateProfileLeds(index) {
    document.getElementById('profile-led-0').classList.toggle('lit', (index & 1) !== 0);
    document.getElementById('profile-led-1').classList.toggle('lit', (index & 2) !== 0);
    document.getElementById('profile-led-2').classList.toggle('lit', (index & 4) !== 0);
}
```

Verify the encoding against `lib/PedalLogic/src/profile_manager.cpp` before finalising.

#### Config validation

Reuse `schema.json` from TASK-032 (`../tools/config-builder/schema.json`). Load it via:

```javascript
const schema = await fetch('../tools/config-builder/schema.json').then(r => r.json());
```

If the fetch fails (e.g. running the file locally without Pages), fall back to a minimal inline
schema that only checks for `profiles` array existence.

### CSS (`styles.css`)

Design goals:

- Dark theme (studio/rack-unit aesthetic: dark grey body, green or amber LEDs)
- Pedal buttons: large enough to click with a foot metaphor, rounded, with a pressed/active state
- `lit` class on `.led` elements → glowing effect (CSS box-shadow)
- Output logs: monospace font, fixed-height scrollable, newest entry at bottom

Key selectors:

```css
.pedal-btn          /* default state */
.pedal-btn:active   /* CSS press visual — show during mousedown */
.pedal-btn.held     /* JS-added class for WhilePressed actions */
.led                /* small circle, dark */
.led.lit            /* glowing circle */
.output-log         /* monospace, dark bg, fixed height, overflow-y: auto */
```

Responsive: on narrow viewports (< 600px), stack the two panels vertically.

### Example config (`example.json`)

Copy `data/profiles.json` verbatim at build time. Add a step to `docs.yml`:

```yaml
- name: Copy example config to simulator
  run: cp data/profiles.json docs/simulator/example.json
```

The "Load Example" button calls `fetch('./example.json')` and passes it to `loadConfig()`.

### README link

Add to the Builder row in `README.md`:

```markdown
| Builder — I want to build one | [Build Guide](docs/builders/BUILD_GUIDE.md) · [Building from Source](docs/building.md) · [3D-printable enclosure](https://www.printables.com/model/1683455-awesomestudiopedal) · [Simulator](https://tgd1975.github.io/AwesomeStudioPedal/simulator/) · [Config Builder](https://tgd1975.github.io/AwesomeStudioPedal/tools/config-builder/) |
```

Or add a dedicated "Musician / no hardware" row:

```markdown
| Musician (no hardware) — I want to try it | [Simulator](https://tgd1975.github.io/AwesomeStudioPedal/simulator/) |
```

## Test Plan

No automated tests required — this is a purely static web tool with no C++ logic.

Manual acceptance testing checklist:

- Load `example.json` via "Load Example"; verify all 7 profiles load and button labels match
  `data/profiles.json`
- Click each button in profile 1; verify BLE Output log shows correct key/string
- Click SELECT 7 times; verify LED encoding cycles 0→1→2→3→4→5→6→0 (binary) and profile name updates
- Click button A in profile 2 (DelayedAction 3000ms); verify countdown appears, output fires after 3 s
- Press and hold a PinHighWhilePressed button; verify "PIN X: HIGH (held)" shows; release; verify
  "PIN X: released" shows
- Load an invalid JSON file; verify error is shown and previous config is preserved
- Click "Export profiles.json"; verify downloaded file opens correctly in the config builder

## Notes

- Verify the exact JSON `type` strings for pin actions in `lib/PedalLogic/src/config_loader.cpp`
  before hardcoding them in `formatActionOutput()`. The `Action::Type` enum uses short names
  (`PinHigh`, `PinLow`…) but the JSON parser may expect different strings (e.g. `PinHighAction`).
- The simulator runs entirely client-side with no backend. It must work when opened as a local
  `file://` URL too (useful for development) — avoid absolute URL paths; use relative ones.
- Delayed actions that are still pending when the page is closed should be cancelled via
  `window.beforeunload` to avoid confusing output on reload.
- Related: TASK-032 (config builder) — share schema.json and export logic.
- Possible future extension (not in scope here): keyboard event listener that maps physical key
  presses to simulator button presses, for hands-free testing.
