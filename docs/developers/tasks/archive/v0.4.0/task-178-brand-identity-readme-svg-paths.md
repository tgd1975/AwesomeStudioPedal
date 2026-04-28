---
id: TASK-178
title: Brand identity — README header and SVG wordmark text-to-paths
status: closed
closed: 2026-04-20
opened: 2026-04-20
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1-low-confidence
complexity: Junior
human-in-loop: None
epic: design
order: 3
---

## Description

Two related housekeeping items that make the brand assets usable everywhere — including
GitHub, where Metropolis is not loaded:

1. **Convert SVG text to paths** — the wordmark SVGs use a `<text>` element with
   `font-family="Metropolis"`. Without Metropolis loaded the text renders in the system
   fallback font. Converting to outlined paths makes the SVGs self-contained and
   renderable anywhere.

2. **Update `README.md`** — add the logo mark, rewrite the intro in brand voice, and
   link to the design system.

## Scope

### 1 — SVG text-to-paths

Files to convert:

- `docs/design/handoff/logo/asp-lockup.svg` — mark + wordmark combined
- `docs/design/handoff/logo/asp-wordmark.svg` — wordmark only

**How:** open each SVG in Inkscape (`Object → Select All`, then `Path → Object to Path`
and `Text → Remove Font`), or use the Inkscape CLI:

```bash
inkscape --export-text-to-path --export-plain-svg \
  --export-filename=asp-lockup.svg asp-lockup.svg
```

Verify after conversion:

- [ ] SVG renders identically at 720 px wide in a browser with no web fonts loaded
- [ ] File contains no `<text>` or `<tspan>` elements (only `<path>`)
- [ ] File contains no `<font>` or `<style>` blocks referencing Metropolis

`asp-mark.svg` and `asp-monogram.svg` are pure geometry — no conversion needed.

### 2 — README update

- Add `asp-mark.svg` as a header image (the mark, not the lockup — the lockup depends on
  Metropolis being loaded until the text-to-paths conversion is done first)
- Rewrite the first paragraph in brand voice: calm, precise, peer-to-peer — no hype,
  no exclamation marks
- Add a row to the directory/navigation table pointing to `docs/design/` with a one-line
  description of the design system

## Acceptance Criteria

- [ ] `asp-lockup.svg` and `asp-wordmark.svg` contain no `<text>` or `<tspan>` elements
- [ ] Both SVGs render correctly in a browser with no external fonts loaded
- [ ] `README.md` opens with the ASP mark SVG image
- [ ] README intro paragraph uses brand voice (no marketing language)
- [ ] `markdownlint` passes on `README.md`

## Notes

- Do not use the lockup SVG directly in the README until text-to-paths is complete;
  use the mark SVG (`asp-mark.svg`) — it is pure geometry and renders everywhere
- Inkscape is the recommended tool; it is available as a snap on Ubuntu
  (`sudo snap install inkscape`)
