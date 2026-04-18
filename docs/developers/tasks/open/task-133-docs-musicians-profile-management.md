---
id: TASK-133
title: Musicians Docs — Profile Management Overview
status: open
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: MobileApp
order: 16
prerequisites: [TASK-131, TASK-132]
---

## Description

Update musician-facing documentation to explain that profiles can be changed without a builder or a cable, using the mobile app or the web builder. Keep it jargon-free — no mention of GATT services, JSON, or BLE characteristics.

## Acceptance Criteria

- [ ] `docs/musicians/USER_GUIDE.md` updated with a "Customising your pedal" section after "Connecting via Bluetooth"
- [ ] Section includes a brief comparison table (Mobile app vs. Web builder + CLI vs. Manual JSON edit) with "Needs computer", "Needs cable", "BLE upload" columns
- [ ] `docs/musicians/PROFILES.md` updated with a "Using the app to edit profiles" section — short UI walkthrough (profile list → button slot → action picker → upload); links to `docs/builders/APP.md` for full detail
- [ ] Language is accessible to non-technical musicians — no jargon, no code snippets

## Files to Touch

- `docs/musicians/USER_GUIDE.md`
- `docs/musicians/PROFILES.md`

## Test Plan

Review only — have a non-technical reader verify the language.

## Prerequisites

- **TASK-131** — CLI doc must exist (musician doc links to it)
- **TASK-132** — app doc must exist (musician doc links to it)
