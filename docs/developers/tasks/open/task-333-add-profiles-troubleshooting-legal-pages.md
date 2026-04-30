---
id: TASK-333
title: Add Profiles explainer, Troubleshooting, and Legal pages
status: open
opened: 2026-04-30
effort: Medium (2-8h)
complexity: Junior
human-in-loop: Clarification
epic: app-content-pages
order: 4
prerequisites: [TASK-331]
---

## Description

Bundle the three remaining short content pages from IDEA-037, since each
is small on its own and they share the same content-page chrome from
TASK-331:

- **Profiles — what they are** — tiny explainer: a profile = a mapping
  from buttons to actions. Pointer to the configurator, not a full
  reference.
- **Troubleshooting** — short list, each entry one line + one suggested
  action ("Pedal not found?" → check power / Bluetooth / distance;
  "Buttons do nothing?" → check active profile).
- **Legal / Open source** — license of the app and firmware; third-party
  licenses auto-generated where possible (`flutter_oss_licenses` or
  `LicenseRegistry`-based screen).

## Acceptance Criteria

- [ ] All three pages reachable from the same content-page entry point.
- [ ] Profiles page links to the configurator screen (in-app
      navigation, not external URL).
- [ ] Troubleshooting page renders at least the two example entries
      from IDEA-037 and is structured so adding more entries is one
      list item.
- [ ] Legal page lists the app license and a generated third-party
      license screen, not a hand-typed list.
- [ ] Each page fits one phone screen without scrolling (Troubleshooting
      may scroll once it grows beyond ~6 entries — acceptable).

## Prerequisites

- **TASK-331** — provides the shared content-page chrome and the
  navigation entry point.

## Test Plan

**Host tests** (Flutter widget tests):

- Add `app/test/pages/profiles_explainer_page_test.dart`,
  `troubleshooting_page_test.dart`, `legal_page_test.dart`.
- Cover: each page renders; Troubleshooting list is data-driven (one
  list-of-records, not three hand-coded `ListTile`s); Legal page
  successfully loads at least one third-party entry from the generated
  source.

## Notes

- The three pages are bundled because individually they are each well
  under S, and they share the same chrome and navigation entry. If any
  one grows in scope (especially Troubleshooting once we hear from real
  users), split it back out as its own task.
- Auto-generation of third-party licenses likely needs a build-time
  step or a runtime walk over `LicenseRegistry`. Pick whichever is
  cheaper to maintain.
