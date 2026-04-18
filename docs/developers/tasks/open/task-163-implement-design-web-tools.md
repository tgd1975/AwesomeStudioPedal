---
id: TASK-163
title: Implement design — web simulator and configurators
status: open
opened: 2026-04-19
effort: Large (8-24h)
complexity: Medium
human-in-loop: None
group: design
---

## Description

Apply the visual design defined in TASK-162 to the three browser-based web tools:

- **Simulator** — `docs/simulator/` (`index.html`, `styles.css`, `simulator.js`)
- **Profile Builder** — `docs/tools/config-builder/` 
- **Configuration Builder** — `docs/tools/configuration-builder/`

**Depends on:** TASK-162 (design tokens and wireframes must be completed first)

## Scope

### Shared design system file

Extract common CSS custom properties (design tokens from TASK-162) into a single shared
stylesheet, e.g. `docs/tools/shared/design-system.css`, and `@import` it from each tool.
This ensures colour, typography, and radius changes propagate everywhere.

### Per-tool changes

#### Simulator (`docs/simulator/`)

- [ ] Apply design token colours — replace hardcoded hex values in `styles.css`
- [ ] Align header toolbar style with the configurators
- [ ] LED indicators: use consistent size, colour, and glow effect across all tools
- [ ] Pedal button visual: ensure consistent style with the other tools' buttons
- [ ] Log / output area: monospace font token applied
- [ ] Navigation links in header: consistent style across all three tools

#### Profile Builder (`docs/tools/config-builder/`)

- [ ] Apply design tokens
- [ ] Align form field styles (inputs, selects, labels) with the design spec
- [ ] Status badge (valid / error) matches the token colours
- [ ] JSON preview panel: use `--font-mono` and `--preview-bg` tokens

#### Configuration Builder (`docs/tools/configuration-builder/`)

- [ ] Apply design tokens (this tool has the most inline CSS — extract to a stylesheet)
- [ ] Align button variants (primary, secondary, danger) with the design spec
- [ ] Validation feedback (error/warning/ok banners) aligned with token colours

### Responsive layout

For each tool, implement the responsive breakpoints from TASK-162 so the tool is
usable on a phone screen (see also TASK-140):

- [ ] Simulator: stack toolbar vertically on narrow viewports
- [ ] Profile Builder: single-column form on narrow viewports
- [ ] Configuration Builder: single-column on narrow viewports

## Acceptance Criteria

- [ ] Shared `design-system.css` file created and imported by all three tools
- [ ] All three tools render consistently with the TASK-162 design spec
- [ ] No hardcoded hex colour values remain in `styles.css` / inline styles (use tokens only)
- [ ] All three tools pass basic responsive check at 375 px viewport width
- [ ] Existing functionality of all three tools is unchanged (visual-only refactor)
- [ ] `markdownlint` and clang-format pre-commit checks still pass (no C++/MD changes expected)

## Notes

- Do not change any JavaScript logic in this task — CSS/HTML only
- Test in Chromium and Firefox (both used by the target audience)
- Low-light / dark appearance is preferred (see TASK-162 design notes re: stage environments)
