---
id: TASK-332
title: Add How-To quickstart page (3–5 steps)
status: open
opened: 2026-04-30
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
epic: app-content-pages
order: 3
prerequisites: [TASK-331]
---

## Description

Add a How-To quickstart page that explains how to use the pedal in 3–5
steps, each one line, each with a screenshot or icon. Reuses the shared
content-page chrome from TASK-331.

Reference flow: power on → pair → pick profile → press button.

## Acceptance Criteria

- [ ] Page reachable from the same content-page entry point as
      Info/About.
- [ ] 3–5 steps total, each one line plus one icon or screenshot.
- [ ] Page fits one phone screen without scrolling on a Pixel 9.
- [ ] If TASK-330 chose "context-aware How-To", the "Pair your pedal"
      step shows or hides based on current connection state. Otherwise
      static.

## Prerequisites

- **TASK-331** — provides the shared content-page chrome and the
  navigation entry point.

## Test Plan

**Host tests** (Flutter widget tests):

- Add `app/test/pages/howto_page_test.dart`.
- Cover: all step rows render; step count is between 3 and 5; (if
  context-aware) the pair step toggles correctly when a connection
  state stream emits connected/disconnected.

## Notes

- Per IDEA-037, this is "reduced to the max" — resist the urge to
  explain edge cases; that's the Troubleshooting page's job.
- First-run auto-show behavior depends on the TASK-330 decision and
  may add a small follow-up step here (read-and-set a "shown once"
  preference) if the answer was "auto-show".
