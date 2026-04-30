---
id: TASK-343
title: Add GitHub social preview image
status: open
opened: 2026-04-30
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
epic: LogoBranding
order: 5
---

## Description

When the repo is shared on Twitter/Discord/Slack, GitHub serves a generic
preview card because no social preview is configured. Add an `og:image` /
`twitter:image` to the gh-pages HTML surfaces (simulator and config builders),
and upload a 1280×640 social preview to the GitHub repo settings so the
repository's own share card uses the ASP mark.

Per IDEA-049 this is the "sanity check" item — small polish with disproportionate
effect on perceived cohesiveness.

## Acceptance Criteria

- [ ] A 1280×640 social preview PNG generated from `asp-mark.svg` (mark + wordmark on background) and committed under `docs/design/handoff/logo/`
- [ ] `og:image` and `twitter:image` meta tags added to simulator and both config builder HTML entry pages, pointing at the committed PNG via an absolute URL (gh-pages)
- [ ] User uploads the same PNG to the repo's GitHub *Settings → Social preview* (human-in-loop step — agent cannot do this)

## Test Plan

No automated tests required — change is non-functional (meta tags + asset).

Manual verification: paste the gh-pages URL into Twitter's Card Validator or
the Open Graph debugger and confirm the preview image renders. After the user
uploads to GitHub Settings, paste the repo URL into the same validator.

## Notes

- The GitHub Settings upload step requires repo admin access via the web UI; the agent should pause on the final AC and ask the user to do that step.
- Asset source: `docs/design/handoff/logo/asp-mark.svg`.
