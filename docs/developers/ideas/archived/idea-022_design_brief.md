# AwesomeStudioPedal — Design Brief

This document is the authoritative brief for generating the corporate identity, design
system, and UX/UI specifications for all user-facing surfaces of the AwesomeStudioPedal
project. It covers target audience, product context, UI surfaces, and design direction.

---

## 1. Product in One Sentence

**AwesomeStudioPedal** is an open-source, programmable Bluetooth foot controller that lets
musicians send keystrokes, media commands, and macros to any device — hands-free, from
the floor.

---

## 2. Target Audience

### 2.1 Primary — The Maker-Musician

**Who they are**

A guitarist, keyboardist, or singer-songwriter who runs a home studio or small rehearsal
space. They are also comfortable with technology: they have flashed firmware before, own
a soldering iron, and follow blogs about DIY audio gear. They think of themselves as
builders as much as musicians. They are the people who buy Raspberry Pi kits, mod their
own pedals, and discuss microcontroller choices on Reddit.

**Age range**: 25–45  
**Gender split**: ~70 % male, 30 % female/non-binary  
**Geography**: Global; heaviest in the US, Germany, the UK, and Japan  
**Income**: Middle income; happy to spend €20–€60 on a project, unwilling to pay €200 for
a commercial equivalent  

**What they want from this product**

- A foot controller they built themselves and can fully customize
- JSON-level control over every button press
- Fast iteration: edit a profile, upload it, try it in the next rehearsal
- A project that looks professional enough to show off to band members

**What they are afraid of**

- Opaque commercial products with subscription lock-in
- Wasted build time if documentation is unclear
- Embarrassment if their DIY tool looks janky next to commercial alternatives

**How they encounter the product**

GitHub search, HackSpace magazine, Hackaday posts, YouTube DIY music builds

---

### 2.2 Secondary — The Non-Technical Musician

**Who they are**

A working musician — cover-band guitarist, classical cellist, solo streaming performer, or
music teacher — who wants the pedal's functionality without the hardware build. They may
ask a maker friend to build the hardware, or buy a pre-assembled unit. Their relationship
with the product is entirely through the mobile app or web configurators.

**Age range**: 20–55  
**Background**: Professional or semi-professional musician, music educator  
**Tech comfort**: Comfortable with smartphones and streaming apps; uncomfortable with
command-line tools or JSON files  

**What they want**

- Change button assignments in a few taps before a gig
- Browse pre-made profiles for their workflow (score navigation, OBS, Zoom)
- Upload changes wirelessly — no cables, no laptop
- Feedback that the change worked ("connected", "uploaded successfully")

**What they are afraid of**

- Breaking something by accident
- The app looking like a developer tool rather than a musician tool
- Steps that require a computer mid-workflow

**How they encounter the product**

Instagram reels, YouTube performances, band recommendations, music forum threads

---

### 2.3 Tertiary — The Hardware Builder

**Who they are**

A hobbyist who focuses on the physical build: sourcing the enclosure, wiring the buttons,
3D-printing the case. They may not write code but they are comfortable with PlatformIO,
reading pinouts, and soldering. They use the web Configuration Builder to map custom GPIO
pin assignments to their non-standard enclosure layout.

**What they want**

- A configuration tool that speaks their language: GPIOs, pin numbers, pull directions
- Validation before they flash — they don't want to reflash 10 times
- Visual confirmation that their pin map is legal

---

### 2.4 Quaternary — The Open-Source Contributor

**Who they are**

A firmware developer, Flutter developer, or web developer who wants to extend the project.
They use the project as a learning platform or portfolio piece. They interact primarily
with the codebase, CI, and documentation.

**What they want**

- Architecture docs that explain design decisions
- A consistent codebase with clear patterns to follow
- Automated tests they can run before opening a PR

---

## 3. Brand Personality and Aesthetic Direction

### 3.1 Brand Personality

| Dimension | This brand | Not this brand |
|-----------|-----------|----------------|
| Tone | Calm, precise, confident | Hype, marketing-speak, casual |
| Vibe | Studio workbench | Consumer electronics store |
| Register | Peer-to-peer (maker to maker) | Brand talking down to customer |
| Texture | Matte, minimal, tactile | Shiny, gradient-heavy, decorative |

The brand should feel like a tool made by someone who uses it on stage. It is serious
without being corporate. It is minimal without being cold.

### 3.2 Visual Aesthetic

**Keyword palette**: Dark studio, warm amber, precision, open-source craftsmanship

- **Base**: Dark theme. Musicians use this in low-light stage environments. High contrast
  is not optional — it is a safety requirement. The product must be legible at a glance
  under stage lighting, from the floor.
- **Accent**: A single warm accent color (amber / orange-yellow range recommended) signals
  active state, connection, and success. This echoes warm LED indicator light colors on
  real hardware.
- **Typography**: Clean sans-serif for UI labels and body copy. A monospace font for all
  JSON previews, serial output, and key names — reinforcing the technical, precise nature
  of the product.
- **Radius**: Slightly rounded corners. Not pill-shaped, not razor-sharp. Consistent
  throughout.
- **Iconography**: Line icons, medium weight, no fill. Avoid solid-fill icon sets — they
  read as decorative rather than functional.
- **Depth**: Minimal shadows. Cards are distinguished from backgrounds by subtle elevation
  (1–2 px border or very low-opacity shadow), not aggressive drop-shadows.

### 3.3 Corporate Identity Elements Required

1. **Logo mark**: An abstract mark that works at 32 px (favicon), 64 px (app icon), and
   large format. Should evoke: a pedal footswitch, Bluetooth symbol, or musical foot.
   Must work in single-color (for etching/embossing on hardware enclosure).
2. **Logotype**: Product name "AwesomeStudioPedal" set in a chosen typeface. May be
   abbreviated "ASP" for compact use.
3. **Color palette**: Primary, surface, background, text, text-muted, success, error,
   warning — with hex values and names.
4. **Typography scale**: Heading 1–3, body, caption, mono — with size, weight, line-height.
5. **Iconography style guide**: Reference icon set and usage rules.
6. **App icon**: Flutter app icon (1024×1024, exportable to all platform sizes).
7. **GitHub social preview**: 1280×640 banner for the repository.

---

## 4. UI Surfaces

### 4.1 Web Simulator (`docs/simulator/`)

**Purpose**

A browser-based, interactive replica of the physical pedal. Visitors can click virtual
buttons, watch the LED cluster respond, and read a live action log — all without owning
hardware. It is the product's primary "try before you build" experience.

**User journey**

1. User arrives from GitHub or a blog post link.
2. They see the simulator pre-loaded with an example profile.
3. They click a button, see an LED change state, and read "sent: KEY_PAGE_DOWN" in the log.
4. They switch profiles with the SELECT button and repeat.
5. They decide to load their own profile and drag-drop a JSON file.

**UI Elements**

| Element | Description |
|---------|-------------|
| **Header bar** | Product logo + "Simulator" label, link to profile builder |
| **Pedal panel** | Visual replica of the hardware enclosure: 4 action buttons (A, B, C, D) + 1 SELECT button. Buttons have pressed/released visual states. |
| **LED cluster** | 3 binary-encoded profile LEDs + 1 power LED + 1 Bluetooth status LED. Each LED is an on/off dot with correct color (green for power, blue for BT, amber for profile). |
| **Profile section** | Current profile name + index. Prev/next arrows or SELECT button cycles profiles. |
| **Action log** | Scrolling list of timestamped entries: "Button A → short press → sent KEY_PAGE_DOWN". Monospace font. Auto-scrolls to bottom. Clear button. |
| **Profile loader** | Drag-and-drop zone or file-open button to load a custom JSON profile. Shows filename and validation status. |
| **Status bar** | Shows "Loaded: default-profiles.json" or error message. |

**Layout**

```
┌──────────────────────────────────────────────┐
│  [Logo]  Simulator              [Load Profile]│
├──────────────────────────────────────────────┤
│                                              │
│   ┌──────────────────────────┐               │
│   │    [LED][LED][LED]        │               │
│   │    [PWR][BT]              │               │
│   │                           │               │
│   │  [  A  ] [  B  ]         │               │
│   │  [  C  ] [  D  ]         │               │
│   │           [SELECT]        │               │
│   │  Profile: Score Navigator │               │
│   └──────────────────────────┘               │
│                                              │
│   ┌──────────────────────────────────────┐   │
│   │ Action Log                    [Clear]│   │
│   │ 14:22:01  A short → KEY_PAGE_DOWN   │   │
│   │ 14:22:03  SELECT → profile 2        │   │
│   └──────────────────────────────────────┘   │
└──────────────────────────────────────────────┘
```

**Responsive behaviour**

- Desktop: pedal panel left, log right (two columns)
- Tablet: pedal panel top, log below
- Mobile: pedal panel top, log below (log shortened to 4 lines, expandable)

---

### 4.2 Web Profile Builder (`docs/tools/config-builder/`)

**Purpose**

A form-driven UI for building button-to-action mappings without writing JSON by hand.
The output is a valid profile JSON file the user can save and upload to the device.

**User journey**

1. User arrives (often from the simulator "edit this profile" link).
2. They see an example profile pre-loaded in the form.
3. They rename the profile, click a button slot, and choose an action type from a dropdown.
4. They configure action parameters (key name, delay, string to type).
5. They see the live JSON preview update on the right.
6. They click Export and save the file.

**UI Elements**

| Element | Description |
|---------|-------------|
| **Header bar** | Logo + "Profile Builder" label, Import / Export buttons |
| **Profile metadata section** | Text field for profile name. Numeric field for profile index (0–6). |
| **Button grid** | 4 cards, one per button (A, B, C, D). Each card expands to show trigger slots. |
| **Trigger slot** | One row per trigger type (short press, long press, double press). Each row has an action-type dropdown + context-specific parameter fields. |
| **Action type dropdown** | Options: None, Send Key, Type String, Media Key, GPIO, Serial, Delayed Action, Macro. Selecting an option shows relevant parameter fields. |
| **Parameter fields** | Text input (key name, string), numeric input (delay ms, pin number), select (GPIO direction, media key). Inline validation with error hint text. |
| **Status badge** | "Valid ✓" / "Invalid ✗" badge in the header, updates on every change. |
| **JSON preview panel** | Read-only, monospace, right column. Syntax-highlighted or plain, auto-updates. Scroll-synced to the active section where possible. |
| **Import button** | Opens file picker for existing JSON. Populates form fields. |
| **Export button** | Downloads the current JSON as `<profile-name>.json`. Disabled when invalid. |
| **Reset button** | Reloads the example profile after a confirmation prompt. |

**Layout**

```
┌───────────────────────────────────────────────────────┐
│  [Logo]  Profile Builder      [Import] [Export ✓]     │
├───────────────────────────────┬───────────────────────┤
│  Profile: [Score Navigator  ] │ {                     │
│  Index:   [0              ]   │   "name": "Score ..., │
│                               │   "buttons": {        │
│  ┌ Button A ───────────────┐  │     "A": {            │
│  │ Short press  [Send Key▼]│  │       "short": {      │
│  │   Key: [KEY_PAGE_DOWN  ]│  │         "type":"key", │
│  │ Long press   [None    ▼]│  │         "key":"KEY_…  │
│  └─────────────────────────┘  │       }               │
│                               │   }                   │
│  ┌ Button B ───────────────┐  │ }                     │
│  │ …                       │  │                       │
│  └─────────────────────────┘  │                       │
└───────────────────────────────┴───────────────────────┘
```

**Responsive behaviour**

- Desktop: form left (55 %), JSON preview right (45 %)
- Tablet: JSON preview collapses to a toggle drawer at the bottom
- Mobile: JSON preview hidden behind a "Preview JSON" button; form full-width

---

### 4.3 Web Configuration Builder (`docs/tools/configuration-builder/`)

**Purpose**

An advanced tool for hardware builders who need to customize GPIO pin assignments, button
counts, and LED counts beyond the firmware defaults. Produces a `hardware-config.json`
file that the CLI tool uploads to the device.

**Target user**: Tertiary audience (hardware builders). Expects users to know what a GPIO
pin is. The UI should be precise and informative, not simplified.

**User journey**

1. Builder opens this tool after wiring their custom enclosure.
2. They enter the number of buttons and LEDs in their build.
3. They assign a GPIO pin number to each button and each LED.
4. The tool validates pin conflicts and range violations in real time.
5. They export the config and upload it with the CLI tool.

**UI Elements**

| Element | Description |
|---------|-------------|
| **Header bar** | Logo + "Configuration Builder" label, Import / Export buttons |
| **Platform selector** | Dropdown: ESP32 / nRF52840. Adjusts valid pin ranges and default assignments. |
| **Button count** | Numeric input (1–8). Adding a count appends a new pin-assignment row. |
| **LED count** | Numeric input (0–8). Same pattern. |
| **Pin assignment table** | One row per button or LED. Columns: Label (A, B, C…), Role (button/LED), GPIO pin number input, Pull direction (UP/DOWN/NONE for buttons), Active level (HIGH/LOW). |
| **Conflict checker** | Inline warning on any row that shares a pin with another row. "Pin 5 already used by Button B." |
| **Range validator** | Inline error if pin number is outside valid range for selected platform. |
| **Status badge** | "Valid ✓" / "Invalid ✗" / "⚠ Warnings" in the header. |
| **JSON preview panel** | Same pattern as Profile Builder — right column, monospace, auto-updates. |
| **Export button** | Downloads `hardware-config.json`. Disabled when invalid. |

**Layout**

Same two-column pattern as the Profile Builder. Form on the left, JSON preview on the
right. The form is more tabular (a table of pin rows) than the Profile Builder.

---

### 4.4 Flutter Mobile App (`app/`)

**Purpose**

The zero-friction configuration interface for non-technical musicians. Connect to the
pedal over Bluetooth, browse pre-made profiles, create or edit profiles, and upload
changes — all from a phone at rehearsal.

**Platform**: Android and iOS. Material 3 (Material You) design system as the underlying
framework, themed to the ASP design system colors.

#### Screen Inventory

##### Home Screen

**Purpose**: Entry point. Shows connection status and available pedals. Primary action is
connecting to a pedal.

**UI Elements**

| Element | Description |
|---------|-------------|
| **App bar** | Product logo/name. Settings icon (top right). |
| **BLE status chip** | Pill badge: "Disconnected" (grey), "Scanning…" (pulsing amber), "Connected to ASP-4A2F" (green). |
| **Scan button** | Primary CTA: "Scan for pedal". Disabled while scanning. Shows spinner during scan. |
| **Device list** | List of discovered pedals (name + signal strength). Tap to connect. |
| **Connected state** | When connected: device name, battery level indicator, "Manage Profiles" and "Edit Hardware Config" cards. |
| **Error state** | Snackbar for BLE errors ("Bluetooth is off", "Permission denied"). |

---

##### Profile List Screen

**Purpose**: Browse all profiles stored on the connected pedal. Select one to edit, or
add a new profile.

**UI Elements**

| Element | Description |
|---------|-------------|
| **App bar** | "Profiles" title, Upload button (cloud-up icon), Back |
| **Profile list** | One card per profile. Shows: index number, profile name, number of mapped actions. Active profile highlighted. |
| **Swipe actions** | Swipe left to delete a profile (with undo snackbar). |
| **Add FAB** | Floating action button: "+" — creates a blank profile and opens Profile Editor. |
| **Upload button** | Uploads all profiles to the device. Shows upload progress (see Upload Screen). |
| **Empty state** | Illustration + "No profiles. Tap + to create one." |

---

##### Profile Editor Screen

**Purpose**: Edit a single profile — name, index, and button-to-action mappings.

**UI Elements**

| Element | Description |
|---------|-------------|
| **App bar** | Profile name (editable inline), Save button, Back (with unsaved-changes dialog) |
| **Profile metadata** | Text field: profile name. Number picker: profile index (0–6). |
| **Button section** | One expandable section per button (A, B, C, D). Tapping expands to show trigger slots. |
| **Trigger row** | One row per trigger (short press, long press, double press). Shows action type label + summary. Tap to open Action Editor. |
| **Add action chip** | Inside each trigger row: "+ Add action" if empty. |
| **Validation banner** | Sticky banner at top: green "Valid" or red "Invalid — tap to see issues". |

---

##### Action Editor Screen

**Purpose**: Configure a single action for one button trigger. This is the deepest editing
level and the most technically complex screen.

**UI Elements**

| Element | Description |
|---------|-------------|
| **App bar** | "Edit Action", Done, Cancel |
| **Action type dropdown** | Full-width dropdown: None / Send Key / Type String / Media Key / GPIO / Serial / Delayed Action / Macro. |
| **Dynamic parameter fields** | Fields change based on action type (see table below). |
| **Validation hints** | Inline helper text below each field. Red text for invalid values. |

**Action type → parameter fields mapping**

| Action type | Fields shown |
|-------------|-------------|
| Send Key | Key name text field with autocomplete (KEY_PAGE_DOWN, KEY_F1, etc.) |
| Type String | Multi-line text input. Character count. |
| Media Key | Dropdown: play, pause, next, previous, volume up, volume down, mute |
| GPIO | Pin number, direction (OUTPUT), active level (HIGH/LOW/TOGGLE), hold-during-press toggle |
| Serial | String to send, baud rate (read-only, shows device default) |
| Delayed Action | Delay (ms) numeric input + nested action picker (same type dropdown, one level deep) |
| Macro | Ordered list of action groups; add/remove/reorder rows |

---

##### Upload Screen

**Purpose**: Upload the edited profile set to the connected pedal. Shown as a modal or
dedicated screen triggered by the Upload button.

**UI Elements**

| Element | Description |
|---------|-------------|
| **Progress bar** | Linear progress, 0–100 %. Updates as chunks are uploaded. |
| **Status label** | "Uploading profile 2 of 5…", "Verifying…", "Done." |
| **Upload log** | Scrolling list of upload steps (optional, collapsible). |
| **Cancel button** | Cancels in-progress upload with confirmation. |
| **Success state** | Green checkmark, "Upload complete. Pedal updated." Dismiss button. |
| **Error state** | Red icon, error message, Retry button. |

---

##### Hardware Config Screen

**Purpose**: Edit the device's GPIO pin assignments. Equivalent to the web Configuration
Builder but on mobile. Accessed from the Home Screen "Edit Hardware Config" card.

**UI Elements**

| Element | Description |
|---------|-------------|
| **App bar** | "Hardware Config", Upload button, Back |
| **Platform chip** | Read-only chip showing detected or selected platform (ESP32 / nRF52840) |
| **Pin assignment list** | One list tile per button and LED. Shows label, current pin number, pull direction. Tap to edit. |
| **Pin editor bottom sheet** | Slides up on tile tap: pin number field, pull direction dropdown, active level dropdown. Confirm / cancel. |
| **Conflict warning chip** | Inline orange chip on conflicting rows: "Shared with Button B" |
| **Validation banner** | Same pattern as Profile Editor. |
| **Upload button** | Uploads hardware config to device. Reboot prompt after success. |

---

## 5. Design Tokens (Required Values)

These tokens must be defined with exact hex values and exported as both CSS custom
properties and Flutter `ThemeData` constants.

### 5.1 Color Palette

| Token name | Role | Suggested value range |
|------------|------|-----------------------|
| `color-primary` | Accent color: buttons, links, active state, BLE connected chip | Warm amber #E8A020 – #F0B030 |
| `color-primary-dim` | Pressed / disabled state of primary | 60 % opacity of primary |
| `color-background` | Page / screen background | Very dark grey #0F0F12 – #12121A |
| `color-surface` | Card / panel background | Slightly lighter than background #1A1A24 – #1E1E2C |
| `color-surface-raised` | Elevated card (dialogs, bottom sheets) | +2 lightness steps above surface |
| `color-border` | Subtle card border / divider | ~15 % white over surface |
| `color-text` | Primary body text | Near-white #E8E8F0 |
| `color-text-muted` | Secondary / helper text | ~50 % opacity of text |
| `color-success` | Valid state, connected, upload done | Green #34C87A |
| `color-error` | Invalid state, error, disconnected | Red #E84040 |
| `color-warning` | Warning, conflict, partial state | Amber (same hue as primary, different context) #E8A020 |
| `color-led-power` | Power LED in simulator | #44FF44 |
| `color-led-bluetooth` | Bluetooth LED in simulator | #4488FF |
| `color-led-profile` | Profile indicator LEDs | #FFB020 |

### 5.2 Typography

| Token name | Role | Suggested value |
|------------|------|-----------------|
| `font-body` | Body copy, labels, form fields | "Inter", "Noto Sans", system-ui |
| `font-mono` | JSON preview, key names, serial output | "JetBrains Mono", "Fira Code", monospace |
| `size-xs` | Caption, badges | 11 px |
| `size-sm` | Helper text, list subtitles | 13 px |
| `size-base` | Body, form labels | 15 px |
| `size-md` | Section headings | 18 px |
| `size-lg` | Screen titles | 22 px |
| `size-xl` | Hero / display | 28 px |
| `weight-regular` | Body | 400 |
| `weight-medium` | Labels, buttons | 500 |
| `weight-bold` | Headings | 700 |
| `line-height-body` | Body text | 1.5 |
| `line-height-heading` | Headings | 1.2 |

### 5.3 Spacing and Shape

| Token name | Value |
|------------|-------|
| `radius-sm` | 4 px (input fields, small chips) |
| `radius-card` | 8 px (cards, panels) |
| `radius-dialog` | 12 px (modals, bottom sheets) |
| `radius-pill` | 999 px (status badges, BLE chip) |
| `space-xs` | 4 px |
| `space-sm` | 8 px |
| `space-md` | 16 px |
| `space-lg` | 24 px |
| `space-xl` | 40 px |

---

## 6. Component Inventory

### 6.1 Web Components (shared across Simulator and both Configurators)

| Component | Current state | Needs |
|-----------|---------------|-------|
| Header / toolbar | Exists, inconsistent | Unify height, color, logo placement |
| Primary button | Exists, blue/default | Restyle to primary amber |
| Secondary button | Exists | Restyle with surface background + border |
| Danger button | Missing | Add red destructive variant |
| Text input | Exists, default browser | Restyle: dark background, border, focus ring |
| Number input | Exists | Same as text input |
| Select / dropdown | Exists, default browser | Custom styled, dark, with chevron |
| Card / panel | Exists, inconsistent | Unify to surface color + radius-card |
| Status badge | Partial (Config Builder) | Extract into reusable component |
| JSON preview panel | Exists in both configurators | Unify: monospace, dark surface, scrollable |
| LED indicator dot | Exists in simulator | Extract to reusable: on/off, color prop |
| Pedal button widget | Exists in simulator | Polish pressed state and label |
| Drag-drop file zone | Exists in simulator | Add hover state, accepted/rejected states |
| Validation banner | Config Builder only | Extract and standardize |

### 6.2 Flutter App Components

| Component | Current state | Needs |
|-----------|---------------|-------|
| App bar | Default Material blue | Re-theme to surface color + primary accent |
| BLE status chip | Missing | Build: pill badge with color + icon |
| Profile list card | Exists, basic | Add profile index badge, action count |
| Action type dropdown | Exists | Re-theme, full-width |
| Parameter form fields | Exists | Input decoration: dark fill, amber focus |
| Validation banner | Partial | Full green/red sticky banner |
| Upload progress bar | Exists | Re-theme to primary amber |
| Bottom sheet | Default Material | Re-theme to surface-raised color |
| Snackbar / toast | Default Material | Re-theme with dark surface |
| Empty state | Missing in places | Illustration + text pattern |
| App icon | Default Flutter | Design and replace |

---

## 7. Responsive Breakpoints (Web Tools)

| Breakpoint name | Min-width | Layout change |
|-----------------|-----------|---------------|
| `mobile` | 0 px | Single column; JSON preview hidden / toggle |
| `tablet` | 600 px | Single column; JSON preview in collapsible drawer |
| `desktop` | 960 px | Two-column: form 55 % + JSON preview 45 % |
| `wide` | 1280 px | Max-width container centered at 1200 px |

---

## 8. Dark Mode Stance

**Decision: Dark mode only for this release.**

Rationale:

- The primary use context is low-light stage and studio environments.
- All target personas benefit from a dark UI.
- Building two themes doubles the implementation effort for design tokens and components.
- The Configuration Builder already uses a dark code preview, creating visual incoherence
  in a light-mode design.
- Re-evaluation for a light mode is deferred to a post-v1.0 release.

**Documentation note for TASK-163 / TASK-164**: Do not build a light mode toggle. Use
`prefers-color-scheme: dark` in CSS as a fallback signal only; the design system enforces
dark as the only supported scheme.

---

## 9. Wireframe Scope

The following screens require annotated wireframes before implementation begins:

| Surface | Screen | Priority |
|---------|--------|----------|
| Simulator | Full layout (header, pedal, profile bar, log) | P1 |
| Profile Builder | Form + JSON preview two-column layout | P1 |
| Configuration Builder | Pin table + JSON preview | P1 |
| App | Home screen (disconnected and connected states) | P1 |
| App | Profile list | P1 |
| App | Profile editor | P1 |
| App | Action editor (Show Key variant + String variant) | P1 |
| App | Upload screen (in-progress and done states) | P2 |
| App | Hardware config | P2 |

Wireframes should be created in Excalidraw (excalidraw.com) or Figma. Export as PNG into
`docs/design/wireframes/`.

---

## 10. Deliverable Checklist

- [ ] Logo mark (SVG, single color + full color)
- [ ] Logotype (SVG)
- [ ] App icon (1024×1024 PNG + source SVG)
- [ ] GitHub social preview banner (1280×640 PNG)
- [ ] Design token table (all tokens with hex values and Flutter equivalents)
  → saved to `docs/developers/DESIGN_SYSTEM.md`
- [ ] CSS custom properties file (`docs/design/tokens.css`)
- [ ] Flutter theme constants file (`app/lib/theme/tokens.dart`)
- [ ] Component inventory (this document, Section 6)
- [ ] Wireframes for all P1 screens → `docs/design/wireframes/`
- [ ] Responsive breakpoint spec (this document, Section 7)
- [ ] Dark mode decision documented (this document, Section 8)
