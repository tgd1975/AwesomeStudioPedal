# Component Specs

Dimensions and behavior for the core components used across the
mobile app and web tools. All numbers in logical pixels.

---

## Button

Three variants: **primary**, **secondary**, **ghost**. A fourth
**danger** variant exists for destructive actions only.

| Token               | Primary             | Secondary           | Ghost               | Danger                |
|---------------------|---------------------|---------------------|---------------------|-----------------------|
| Background          | `--asp-accent`      | `--asp-surface`     | transparent         | transparent           |
| Foreground          | `--asp-accent-ink`  | `--asp-text`        | `--asp-text`        | `--asp-error`         |
| Border              | `--asp-accent`      | `--asp-border-strong` | `--asp-border`    | `rgba(232,64,64,0.4)` |
| Hover bg            | +4% lightness       | `--asp-surface-raised` | `rgba(255,255,255,0.04)` | `rgba(232,64,64,0.08)` |

- **Padding:** web `9px 14px`, mobile `10px 16px`
- **Radius:** `--asp-radius-sm` (2 px)
- **Label:** JetBrains Mono 500, 11 px web / 12 px mobile,
  letter-spacing `0.1em`, `text-transform: uppercase`
- **Icon + label gap:** 8 px
- **Min hit target:** 44 × 44 px on mobile

## Input

- **Height:** web 36 px, mobile 44 px
- **Padding:** web `9px 12px`, mobile `12px 14px`
- **Background:** `--asp-bg` (sunken from surrounding surface)
- **Border:** 1 px `--asp-border`; focus → 1.5 px `--asp-accent`
- **Radius:** `--asp-radius-sm`
- **Value:** Metropolis 400, 13 px (or Mono for code/id fields)
- **Label above** (not floating): Mono 10 px, tracking 0.12em, uppercase,
  `--asp-text-muted`, `margin-bottom: 6px`
- **Hint** (optional): same styling as label, right-aligned or
  appended with a middle-dot separator

## Select

- Same shell as Input, with a `▾` glyph at the right edge in
  `--asp-text-muted`, 10 px
- Dropdown: `--asp-surface-raised`, 1 px `--asp-border-strong`,
  `--asp-radius-dialog`, 8 px padding per option

## Card / Panel

- **Background:** `--asp-surface`
- **Border:** 1 px `--asp-border`
- **Radius:** `--asp-radius-card` (4 px)
- **Padding:** 18–24 px depending on density
- **Section head inside:** Mono 10–11 px label tracked 0.15em, prefixed
  `—` em-dash, followed by optional hint in body type

## Status chip

Pill containing a dot + label. Used for connection state, validation,
profile activity.

- **Background:** `rgba(tone, 0.1)`
- **Border:** 1 px `rgba(tone, 0.3)`
- **Foreground:** `tone` colour
- **Dot:** 8 px circle, matching tone, with `box-shadow: 0 0 6px tone`
- **Label:** Mono 11 px, tracking 0.08em, uppercase
- **Padding:** `6px 10px`, **gap:** 8 px
- **Radius:** `--asp-radius-pill`

Tones: `success` (green), `warning` (amber), `error` (red),
`info` (`--asp-led-bluetooth`).

## LED dot

Small circular status indicator mimicking the hardware LEDs.

- **Sizes:** 8, 10, 12 px
- **On:** filled with tone + `box-shadow: 0 0 8px tone,
  inset 0 0 2px rgba(0,0,0,0.3)`
- **Off:** `#222` fill + `inset 0 1px 2px rgba(0,0,0,0.5)`
- **Optional label:** Mono 10 px, tracking 0.08em, uppercase,
  `--asp-text-muted`

## Pedal button (web simulator)

The virtual action buttons A/B/C/D on the Simulator page.

- **Size:** 104 × 104 px
- **Border:** 2 px — matches fill when active, else `--asp-border-strong`
- **Radius:** `--asp-radius-md`
- **Shadow (idle):** `inset 0 -4px 0 rgba(0,0,0,0.3),
                      inset 0 1px 0 rgba(255,255,255,0.06)`
- **Shadow (active):** `0 0 24px tone80,
                         inset 0 -4px 0 rgba(0,0,0,0.2),
                         inset 0 1px 0 rgba(255,255,255,0.15)`
- **Label:** Metropolis 700, 36 px (letter A/B/C/D)
- **Subtitle:** Mono 9 px, 4 px below the label, tracked 0.1em, uppercase

Per-button tones:
`A = #F0B030`  `B = #E84545`  `C = #45C87A`  `D = #6A88FF`

## JSON pane

Split-view right panel used by both builders.

- **Background:** `--asp-surface`
- **Left border:** 1 px `--asp-border`
- **Header:** Mono 10 px label tracked 0.15em, actions right-aligned
- **Code:** Mono 12.5 px, line-height 1.55
- **Key highlight:** `--asp-accent`
- **String value:** `#7FD4A8`   (green)
- **Number:** `#7FB0D4`         (blue)
- **Bool / null:** `#C07FD4`    (purple)
- **Punctuation:** `--asp-text-muted`
- **Status strip (bottom):** dot + label, green when valid,
  red when invalid
