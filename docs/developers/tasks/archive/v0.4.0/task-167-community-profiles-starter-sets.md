---
id: TASK-167
title: Write 12 starter profile sets
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Medium (2-8h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Junior
human-in-loop: Main
epic: CommunityProfiles
order: 2
---

## Description

Create the 12 starter profile set files listed in IDEA-017 "Starter Profiles to Ship at
Launch". Six are extracted from `docs/simulator/example.json`; six are written new. Each
file must be a valid `profiles.json` (per `data/profiles.schema.json`) with a `_meta` block
containing `id`, `author`, `tags`, `tested`, and optionally `minFirmware`.

## Acceptance Criteria

- [ ] All 12 files created in the correct subfolder (determined by `minButtons`):
  - `1-button/shutter-delayed.json`
  - `1-button/play-pause-media.json`
  - `2-button/page-turner-basic.json`
  - `2-button/daw-transport-minimal.json`
  - `2-button/zoom-mute-toggle.json`
  - `2-button/pixel-camera-remote.json`
  - `2-button/vlc-practice.json`
  - `2-button/logic-pro-tracking.json`
  - `3-button/podcast-control.json`
  - `4-button/score-navigator.json`
  - `4-button/obs-stream-deck.json`
  - `4-button/ableton-looper.json`
  - `4-button/discord-comms.json`
  - `4-button/lightroom-culling.json`
- [ ] Each file validates against `data/profiles.schema.json`
- [ ] Each file has a `_meta` block with at minimum `id`, `author`, `tags`
- [ ] Button labels are descriptive (action name + key, not just "Button A")
- [ ] `description` field in each profile explains the app/context and where to configure hotkeys

## Notes

Source for the 6 extractions: `docs/simulator/example.json`.
See IDEA-017 "Profile File Format" for the exact `_meta` structure including the optional
`minFirmware` field. Omit `minFirmware` for all starter profiles (they use only basic action
types available in any firmware version).

Depends on: TASK-166 (folder structure must exist).
