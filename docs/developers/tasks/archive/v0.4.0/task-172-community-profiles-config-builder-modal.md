---
id: TASK-172
title: Web Config Builder — community profiles gallery modal
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Medium (2-8h)
effort_actual: Small (<2h)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: CommunityProfiles
order: 7
---

## Description

Add a "Browse community profiles" button to `docs/tools/config-builder/index.html`. On
click, fetch `profiles/index.json` from GitHub Pages
(`https://tgd1975.github.io/AwesomeStudioPedal/profiles/index.json`), render a modal
with profile cards, and wire card clicks to the existing `loadFromJson()` function — no
new parser needed.

Card content: name, description, tags, `minButtons` badge, `profileCount` (e.g. "3
profiles"), `minFirmware` warning if set, author.

## Acceptance Criteria

- [ ] "Browse community profiles" button visible in the config builder UI
- [ ] Modal fetches `index.json` on open and renders one card per entry
- [ ] Cards show: name, description, tags, minButtons badge, profileCount, author
- [ ] Cards with `minFirmware` set show a visible warning badge
- [ ] Clicking a card fetches the full profile set JSON and calls the existing `loadFromJson()` — same code path as the manual file loader
- [ ] Loading spinner shown while fetching; error state shown if the fetch fails (network offline, CORS error, etc.)
- [ ] Modal can be dismissed without loading a profile

## Notes

The GitHub Pages URL is `https://tgd1975.github.io/AwesomeStudioPedal/profiles/`. CORS
headers are provided by GitHub Pages, so no proxy is needed.

Extract the fetch + render logic into a shared JS module (`profiles-gallery.js`) so it
can be reused by the simulator in TASK-174.

Depends on: TASK-168 (index.json format defined), TASK-167 (profiles available on Pages for testing).
