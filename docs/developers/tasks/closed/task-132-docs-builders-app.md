---
id: TASK-132
title: Builders Docs — Mobile App Walkthrough
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: MobileApp
order: 15
prerequisites: [TASK-145]
---

## Description

Document the mobile app for builders in `docs/builders/APP.md`. Covers installation, BLE pairing, profile editing, uploading, and import/export.

## Acceptance Criteria

- [ ] `docs/builders/APP.md` created
- [ ] Section 1: **Installing** — Play Store link (placeholder); sideloading APK from GitHub Releases
- [ ] Section 2: **Connecting to the pedal** — pairing the config GATT service (separate from HID keyboard pair); what the scan screen shows
- [ ] Section 3: **Editing profiles** — walkthrough of each screen with screenshots (or placeholder for screenshots)
- [ ] Section 4: **Uploading** — progress bar meaning; what to do if upload fails (retry, check LED state)
- [ ] Section 5: **Importing / exporting JSON** — backing up a config; restoring from a file
- [ ] Section 6: **Hardware config tab** — editing pin assignments without recompiling
- [ ] Section 7: **iOS notes** — where iOS behaviour differs from Android (permission prompts, dual BLE service)
- [ ] `docs/builders/` index updated

## Files to Touch

- `docs/builders/APP.md` (new)
- `docs/builders/` index file (if exists)

## Test Plan

Review only.

## Prerequisites

- **TASK-145** — upload screen must be final before the upload section can be written accurately
