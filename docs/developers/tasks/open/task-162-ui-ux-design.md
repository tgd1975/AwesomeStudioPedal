---
id: TASK-162
title: Create UI/UX design for simulator, configurators, and mobile app
status: open
opened: 2026-04-19
effort: Large (8-24h)
complexity: High
human-in-loop: Main
group: design
---

## Description

Define a cohesive visual and interaction design for all three surfaces that make up the
AwesomeStudioPedal user-facing toolset:

1. **Simulator** (`docs/simulator/`) — browser-based pedal emulator
2. **Configurators** — Profile Builder (`docs/tools/config-builder/`) and
   Configuration Builder (`docs/tools/configuration-builder/`)
3. **Mobile app** (`app/`) — Flutter Android + iOS

Currently each surface was built independently and has inconsistent visual language
(colours, typography, spacing, component styles). This task produces a design spec that
the implementation tasks (TASK-163, TASK-164, TASK-165) can follow.

## Deliverables

### 1 — Design tokens (shared variables)

Define a minimal set of design tokens usable by both the web tools (CSS custom properties)
and the Flutter app (theme constants):

| Token | Purpose |
|-------|---------|
| `--color-primary` | Main accent (buttons, links, active state) |
| `--color-surface` | Card / panel background |
| `--color-background` | Page background |
| `--color-text` | Body text |
| `--color-text-muted` | Secondary / helper text |
| `--color-success` | Valid / connected state |
| `--color-error` | Invalid / error state |
| `--color-warning` | Warning state |
| `--radius-card` | Card corner radius |
| `--font-body` | Body font stack |
| `--font-mono` | Monospace (JSON preview, serial output) |

### 2 — Component inventory

For each surface, list which components exist and need visual alignment:

**Web tools (simulator + configurators):**
- Header / toolbar strip
- Primary button, secondary button, danger button
- Form fields (text input, number input, select / dropdown)
- Card / panel container
- Status badge (valid ✓ / error ✗ / warning)
- JSON preview panel (monospace, syntax-highlighted or plain)
- LED indicator (on/off dot)
- Pedal button (clickable hardware button visual)

**Mobile app (Flutter):**
- Home screen cards
- List tiles (profile list, action list)
- Bottom sheet / dialog
- Dropdown form field (action type picker)
- Validation banner (green / red)
- BLE status chip
- Upload progress bar
- JSON preview screen

### 3 — Screen mockups or wireframes

At minimum, produce annotated wireframes (pen-and-paper scan, Excalidraw, Figma, or similar)
for:

- [ ] Simulator — full layout including header toolbar, pedal panel, profile section, log area
- [ ] Profile Builder — form layout, JSON preview side-by-side
- [ ] Configuration Builder — hardware config form, validation feedback
- [ ] App: Home screen
- [ ] App: Profile list screen
- [ ] App: Profile editor screen
- [ ] App: Action editor screen (showing one action type variant)
- [ ] App: Upload screen

### 4 — Responsive / mobile-web considerations

The web tools should work on a phone screen (see TASK-140 — mobile-responsive tools).
The design spec must state breakpoints and describe how each tool adapts.

### 5 — Dark mode stance

Decide: dark mode support yes/no for this release. Document the decision.
The Configuration Builder already has a dark code preview (`--preview-bg: #1e1e2e`); the
Simulator uses a light theme. If dark mode is deferred, document it explicitly.

## Acceptance Criteria

- [ ] Design token table (CSS variables + Flutter equivalents) documented in a new file
  `docs/developers/DESIGN_SYSTEM.md`
- [ ] Component inventory complete — lists what exists and what needs restyling
- [ ] Wireframes / mockups created for all 8 screens listed above
- [ ] Responsive breakpoints defined for web tools
- [ ] Dark mode decision documented

## Notes

- This task is intentionally output-only (docs + wireframes). No code changes.
  Implementation is split into TASK-163 (web tools), TASK-164 (Flutter app), TASK-165 (design system CSS).
- A free Figma account is sufficient; alternatively Excalidraw (excalidraw.com) works offline.
- Aim for a "calm tool" aesthetic — dark background, minimal chrome, clear status indicators.
  Musicians use this in low-light stage environments.
