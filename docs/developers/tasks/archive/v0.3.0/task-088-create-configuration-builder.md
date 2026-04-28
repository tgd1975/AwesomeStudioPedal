---
id: TASK-088
title: Create hardware configuration builder
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
epic: Simulator
order: 3
---

## Description

**Terminology used throughout this project:**

| Term | Meaning |
|---|---|
| **Configuration** (`config.json`) | Hardware layout — how many buttons, how many profile-select LEDs, pin assignments |
| **Profile** (`profiles.json`) | Logical mapping — what each button does in a given profile |

Currently `docs/tools/config-builder/` (the *profile builder*) edits `profiles.json`. There
is no web tool for editing the hardware configuration. Builders who want to change the number
of buttons or LED count must edit firmware source directly.

Create a new static HTML/JS page at `docs/tools/configuration-builder/index.html` that
lets a builder visually configure the `HardwareConfig` struct from `include/config.h` and
download it as `config.json`. The firmware's config loader (or a future extension of it) can
then read this file from the SPIFFS data partition.

### HardwareConfig fields to expose

| Field | Type | Range | Notes |
|---|---|---|---|
| `numButtons` | integer | 1–26 | Controls which slots A–Z are active |
| `numSelectLeds` | integer | 0–6 | Max profiles = 2ⁿ − 1 |
| `numProfiles` | integer | 1–7 | Must be ≤ 2^numSelectLeds − 1 |
| `ledPower` | integer | 0–39 | GPIO pin |
| `ledBluetooth` | integer | 0–39 | GPIO pin |
| `ledSelect[0..5]` | integer | 0–39 | One field per active select LED |
| `buttonSelect` | integer | 0–39 | GPIO pin for SELECT button |
| `buttonPins[A..Z]` | integer | 0–39 | One field per active button |

The page should also include a **rename of the existing profile builder** cross-link:
the link back should say "Profile Builder" not "Config Builder" to match the terminology
above.

## Acceptance Criteria

- [ ] Page exists at `docs/tools/configuration-builder/index.html`
- [ ] All `HardwareConfig` fields listed above are editable
- [ ] `numButtons` controls which `buttonPins` rows are shown (A–Z, hide inactive)
- [ ] `numSelectLeds` controls which `ledSelect` rows are shown
- [ ] `numProfiles` is validated to not exceed 2^numSelectLeds − 1; an inline warning
      is shown if violated
- [ ] Download button produces a `config.json` matching the `HardwareConfig` field names
- [ ] **Load existing** button allows loading a previously downloaded `config.json`
- [ ] **Load Example** button loads a built-in example matching the default ESP32 wiring
      from `src/config_esp32.cpp` (no network fetch — inline data)
- [ ] Page links to the simulator (`../../simulator/`) and the profile builder
      (`../config-builder/`) in the header
- [ ] The existing profile builder (`docs/tools/config-builder/index.html`) updated:
      - Title/subtitle uses "Profile Builder" / "profiles.json" terminology
      - Header navigation link points to this new configuration builder
- [ ] No CDN dependencies — all JS is either inline or vendored locally
- [ ] Works when opened directly from the filesystem (`file://`) with no server

## Test Plan

1. Open `docs/tools/configuration-builder/index.html` from the filesystem.
2. Set `numButtons = 2`, verify only slots A and B are shown in the button-pins section.
3. Set `numSelectLeds = 2`, verify only `ledSelect[0]` and `ledSelect[1]` rows are shown,
   and that setting `numProfiles > 3` triggers a warning.
4. Click **Download** → verify the downloaded `config.json` contains all expected fields.
5. Reload the page, click **Load existing**, select the downloaded file → verify all fields
   repopulate correctly.
6. Click **Load Example** → verify it populates without a network request (test offline).

## Notes

- Keep the tool static HTML/JS — no build pipeline, no npm.
- Match the visual design language of the profile builder (`docs/tools/config-builder/`)
  for a consistent feel.
- The `config.json` format does not need to match the C struct byte-for-byte — it is a
  human-readable JSON that a future firmware extension or flash script will parse.
