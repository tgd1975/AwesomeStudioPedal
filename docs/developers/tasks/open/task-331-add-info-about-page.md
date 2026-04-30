---
id: TASK-331
title: Add Info/About page to the Flutter app
status: open
opened: 2026-04-30
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
epic: app-content-pages
order: 2
prerequisites: [TASK-330]
---

## Description

Add an Info/About page to the Flutter app, reachable from a single entry
point (top-bar info icon or overflow menu — pick once, reuse for all
content pages). The page is the first content surface added under
EPIC-023, so it also establishes the shared "content page" chrome that
the How-To, Profiles explainer, Troubleshooting, and Legal pages will
reuse.

What the page shows:

- One-paragraph project pitch (what the pedal is, why it exists).
- Version, build, firmware-compatibility range — sourced once from a
  central `AppInfo` provider so the splash and Connection details sheet
  use the same string.
- Project links (tappable, open in browser): GitHub repository, project
  website / docs site, release notes / changelog, "Report a bug" →
  GitHub issues.
- License and credits line (full third-party licenses live on the Legal
  page — TASK-335).

## Acceptance Criteria

- [ ] Info/About page reachable from the chosen single entry point.
- [ ] Version string sourced from `package_info_plus` (or equivalent),
      not hard-coded — same source the splash and Connection details
      sheet will read.
- [ ] All external links open in the system browser via `url_launcher`.
- [ ] Page fits one phone screen without scrolling on a Pixel 9 form
      factor.
- [ ] Shared content-page chrome (scaffold + back nav + typography) is
      extracted into a reusable widget the later content tasks can use.

## Prerequisites

- **TASK-330** — content source-of-truth and i18n decisions; determines
  whether copy lives in Dart strings or a Markdown asset, and whether
  i18n is scaffolded now.

## Test Plan

**Host tests** (Flutter widget tests, `flutter test`):

- Add `app/test/pages/info_about_page_test.dart`.
- Cover: page renders without overflow; version string matches
  `AppInfo.version`; tapping a link invokes `url_launcher`'s
  platform-channel mock with the expected URL.

## Documentation

- `docs/users/` — short pointer that the app has an Info/About page (if
  the user-docs decision in TASK-330 lands on "single source of truth").

## Notes

- Hold the choice of icon (info vs overflow) until this task — once
  picked, all subsequent content-page tasks reuse it.
- Keep copy short and factual per IDEA-037's "no marketing copy" rule.
