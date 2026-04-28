---
id: TASK-163
title: Implement design — web simulator and configurators
status: closed
closed: 2026-04-20
opened: 2026-04-19
effort: Large (8-24h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1-low-confidence
complexity: Medium
human-in-loop: None
epic: design
order: 1
---

## Description

Apply the completed design system (tokens, typography, dark palette) to the three
browser-based web tools. The design tokens are in `docs/design/handoff/tokens.css`.
The component spec is in `docs/design/handoff/components/COMPONENTS.md`.

**Important:** the Profile Builder and Configuration Builder are currently **light mode**
(white background, blue accent). This task is a full light→dark conversion for both,
not just a token swap.

## Scope

### 1 — Shared stylesheet

Create `docs/tools/asp.css` that:

- `@import`s `../../design/handoff/tokens.css` (relative path from `docs/tools/`)
- Loads **Metropolis** (display + body font) and **JetBrains Mono** (mono font) — see
  Font Sourcing below
- Defines shared component classes: `.asp-header`, `.asp-btn-primary`, `.asp-btn-secondary`,
  `.asp-btn-danger`, `.asp-input`, `.asp-select`, `.asp-card`, `.asp-status-badge`,
  `.asp-json-preview`

### 2 — Font sourcing (licensing note)

Both fonts are SIL OFL licensed — freely usable in this open-source project.

| Font | Source | Strategy |
|------|--------|----------|
| **Metropolis** | OFL — [github.com/chrismsimpson/Metropolis](https://github.com/chrismsimpson/Metropolis) | Bundle WOFF2 files under `docs/fonts/metropolis/`; `@font-face` in `asp.css`. Google Fonts does not host Metropolis. |
| **JetBrains Mono** | OFL — Google Fonts | `@import` from Google Fonts CDN in `asp.css` |

Download Metropolis WOFF2 from the GitHub releases (Regular, Medium, SemiBold, Bold weights).

### 3 — Simulator (`docs/simulator/`)

- Extract all hardcoded values from `styles.css` to token variables
- Replace `--accent: #4ade80` (green) with `var(--asp-accent)` (amber)
- Replace `--bg: #1a1a2e` family with the ASP palette tokens
- Apply Metropolis to body, JetBrains Mono to the action log
- LED indicators: use `--asp-led-power`, `--asp-led-bluetooth`, `--asp-led-profile`
- Align header toolbar height and border with `asp.css`
- Import `asp.css` alongside existing `styles.css`

### 4 — Profile Builder (`docs/tools/config-builder/`)

- Extract inline `<style>` block into a separate `config-builder.css`
- Import `../asp.css` from that file
- Convert from light mode (`--bg: #f5f5f5`) to dark (`var(--asp-bg)`)
- Replace `--accent: #2563eb` (blue) with `var(--asp-accent)` (amber)
- Restyle: form inputs, selects, cards, status badge, JSON preview panel
- Load Metropolis + JetBrains Mono via `asp.css` (remove any existing font imports)

### 5 — Configuration Builder (`docs/tools/configuration-builder/`)

Same pattern as Profile Builder:

- Extract inline `<style>` block into `configuration-builder.css`
- Import `../asp.css`
- Convert light→dark, blue→amber
- Restyle: pin-assignment table, conflict warning chips, validation banners, JSON preview

## Acceptance Criteria

- [ ] `docs/tools/asp.css` created; all three tools import it
- [ ] `docs/fonts/metropolis/` contains WOFF2 files; loaded via `@font-face` in `asp.css`
- [ ] No hardcoded hex colour values remain in any tool's CSS (token variables only)
- [ ] All three tools are dark-mode only; no white/light backgrounds remain
- [ ] Amber accent replaces blue in all interactive elements
- [ ] Metropolis renders as body/display font; JetBrains Mono in all JSON/log panels
- [ ] Existing JavaScript functionality of all three tools is unchanged
- [ ] Spot-checked in Chromium and Firefox at 1280 px and 375 px viewport widths

## Notes

- CSS/HTML only — no JavaScript logic changes in this task
- Do not introduce a light-mode toggle; dark is the only supported scheme
- WOFF2 is the only format needed (all modern browsers support it)
