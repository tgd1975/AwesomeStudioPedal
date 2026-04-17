---
id: TASK-086
title: Fix "Load Example" NetworkError in simulator
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
group: Simulator
order: 1
---

## Description

Pressing **Load Example** in the simulator (`docs/simulator/index.html`) logs:

```
ERROR: Could not load example — NetworkError when attempting to fetch resource.
```

The root cause is `simulator.js` using `fetch('./example.json')`, which fails when the page
is served from GitHub Pages because the file path resolves incorrectly relative to the
published URL. The `example.json` file exists locally but is not reliably reachable via a
relative `fetch` call in all hosting contexts.

Fix the bug so that clicking **Load Example** always works, both in GitHub Pages and when
opening the file locally in a browser.

## Acceptance Criteria

- [ ] Clicking **Load Example** in the simulator loads `example.json` without an error in both
      GitHub Pages and local `file://` contexts
- [ ] The example is the existing `docs/simulator/example.json` content (no content change needed)
- [ ] Error handling still present for any genuine load failures
- [ ] No regressions to the manual "Load profiles.json" file-picker flow

## Implementation Notes

Preferred approach: embed the example JSON as an inline JS constant inside `simulator.js`
(or a separate `example-data.js`) so no network fetch is needed. This also makes the
simulator work fully offline and avoids all path-resolution issues.

Alternative: use an absolute path anchored to the repository's GitHub Pages base URL
(fragile — depends on deployment URL) or inline the JSON into the HTML as a `<script>`
block with `type="application/json"`.

## Test Plan

1. Open `docs/simulator/index.html` directly from the filesystem (`file://` URL).
2. Click **Load Example** → verify config loads and buttons are labelled.
3. Deploy to GitHub Pages (or use a local HTTP server) → repeat step 2.
4. Verify the serial/BLE log shows no error and `config-name` span reads `example`.
