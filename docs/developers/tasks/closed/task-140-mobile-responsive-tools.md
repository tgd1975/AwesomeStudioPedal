---
id: TASK-140
title: Mobile-Responsive Layout for Simulator and Configurator Tools
status: closed
closed: 2026-04-23
opened: 2026-04-17
effort: Large (8-24h)
effort_actual: Medium (2-8h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: Main
---

## Description

The three web tools — the **Simulator** (`docs/simulator/`), the **Profile Builder** (`docs/tools/config-builder/`), and the **Configuration Builder** (`docs/tools/configuration-builder/`) — have only been tested on a notebook. They need to be usable on a smartphone (portrait and landscape) so musicians and builders can configure the pedal on any device.

The tools currently have partial breakpoints at 600–800 px, but several layout and interaction problems remain on phones:

- **Profile Builder and Configuration Builder:** `height: calc(100vh - 57px)` locks the layout to the visible viewport. On mobile the form panel and JSON preview panel stack vertically, but the preview gets a fixed `min-height: 400px` which wastes screen space on short phones.
- **Profile Builder and Configuration Builder:** The header toolbar wraps but navigation links lose their right-alignment before the breakpoint, creating an awkward mid-width state.
- **Profile Builder:** The buttons grid (`grid-template-columns: 72px 1fr`) gives a reasonable label column on desktop but the action editor fills most of the width — fine for wide screens, crowded on 360 px wide phones.
- **Simulator:** The 600 px breakpoint switches to single-column, but pedal buttons have `min-height: 90px` and a 2-column grid — on a phone with ≥ 4 buttons the grid is fine, but the output log panel needs a minimum usable height so it isn't hidden below the fold.
- **All three tools:** No touch-specific tap-target sizing. Interactive elements (tabs, dropdowns, sub-action `<details>` summaries, number inputs) may be below the recommended 44×44 px minimum tap target size on mobile.
- **Profile Builder:** The `<details>` sub-action panels for Long Press and Double Press have a small `summary` — easy to miss-tap on a touchscreen.
- **All three tools:** Hover styles (`button:hover`) are harmless on mobile but `:active` feedback should be sufficient; the held/active state on simulator buttons may not register well on some touch browsers.

## Acceptance Criteria

- [ ] All three tools are usable on a 360×800 px portrait phone screen (Android baseline) without horizontal scrolling
- [ ] All three tools are usable on a 375×667 px portrait screen (iPhone SE baseline)
- [ ] Form controls (inputs, selects, dropdowns, `<details>` summaries, tab buttons) meet a minimum tap-target height of 44 px on mobile
- [ ] The Profile Builder and Configuration Builder collapse to a tabbed or toggled single-column layout on mobile: form first, JSON preview accessible via a "Show JSON" toggle button (replaces the fixed side-by-side split)
- [ ] The Simulator's pedal buttons are large enough to tap reliably (min 64×64 px on mobile)
- [ ] Output logs in the Simulator have a visible, scrollable minimum height on mobile
- [ ] The header nav links remain accessible on mobile (no overflow clipping)
- [ ] Landscape orientation on phone is usable — content scrolls rather than overflows
- [ ] No regressions on desktop (≥ 1024 px wide)

## Files to Touch

- `docs/simulator/styles.css` — button tap targets, output log min-height, touch improvements
- `docs/simulator/index.html` — structural changes if needed for mobile layout
- `docs/tools/config-builder/index.html` — mobile layout, form/preview toggle, tap targets, `<details>` sub-action sizing
- `docs/tools/configuration-builder/index.html` — same as config-builder

## Implementation Notes

**Form/preview toggle (Profile Builder and Configuration Builder):**
Replace the static side-by-side grid with a mobile-first approach. On narrow screens (< 800 px), show only the form panel by default. Add a sticky "Show JSON ▾" button at the bottom of the form that slides the preview panel into view (or uses a simple `display: none` toggle). The preview panel already has a toolbar — that toolbar can house the toggle button on mobile.

**Tap targets:**
Add a mobile-specific rule for all interactive elements:

```css
@media (max-width: 800px) {
  input, select, textarea, button, summary { min-height: 44px; }
  .tab-btn, .sub-action-summary { padding: 10px 14px; }
}
```

**Simulator buttons:**
Increase `min-height` at the 600 px breakpoint so buttons remain easy to tap:

```css
@media (max-width: 600px) {
  .pedal-btn { min-height: 72px; padding: 16px 8px; }
}
```

**Output logs (Simulator):**
On mobile, collapse the three log panels into a tabbed view (BLE / Serial / IO) so each gets full width, or give each a fixed scrollable height (e.g. `min-height: 120px; max-height: 200px`) so they don't push buttons off screen.

**`<details>` sub-action panels (Profile Builder):**
Ensure the `<summary>` element meets the 44 px tap target:

```css
@media (max-width: 800px) {
  .sub-action-summary { padding: 12px 14px; }
}
```

**Viewport meta:**
All three tools already have `<meta name="viewport" content="width=device-width, initial-scale=1.0">` — no change needed there.

## Test Plan

**Manual test (required — `human-in-loop: Main`):**

Test on a real phone or browser DevTools device emulation at 360×800 and 375×667 (portrait), and 667×375 (landscape):

1. **Simulator:** Load the example profile. Tap each pedal button — it should respond clearly. Scroll down to see all output logs. Switch profiles with SELECT.
2. **Profile Builder:** Open the tool. Set a key action on Button A. Open the Long Press sub-action panel and set a media key. Verify the JSON preview is accessible via the toggle. Download the JSON.
3. **Configuration Builder:** Change a pin number. Verify the JSON preview is accessible. Download the config.
4. Rotate to landscape — verify no horizontal scrollbar appears and content scrolls vertically.
5. Test on desktop at 1280 px wide — verify no visual regressions.

No automated tests — this is a pure UI/layout change.

## Notes

This task intentionally defers deep mobile UX work (e.g. native-app-style navigation, swipe gestures) — the goal is "usable without frustration", not a full mobile-first redesign. If the layout work reveals that a specific tool needs deeper structural changes, raise a follow-up task rather than expanding this one's scope.

The human-in-loop value is `Main` because the acceptance criteria require physical testing on a touch device that only the developer can perform.
