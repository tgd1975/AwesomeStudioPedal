---
id: TASK-087
title: Fix Ajv schema validator CDN unavailability in profile builder
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: Clarification
epic: Simulator
order: 2
---

## Description

The profile builder (`docs/tools/config-builder/`) shows:

```
Schema validator not available (check CDN connectivity)
```

The page loads Ajv from `https://cdn.jsdelivr.net/npm/ajv@8/dist/ajv.min.js`. This fails
when the user is offline, when the CDN is blocked (corporate firewall, restricted network),
or when GitHub Pages CSP headers prevent loading from external origins.

The tool's HTML even includes a comment acknowledging this and suggesting to vendor the file
locally — that fix was never implemented.

Vendor `ajv.min.js` locally so schema validation works offline and in all network
environments.

## Acceptance Criteria

- [ ] `ajv.min.js` (Ajv v8) checked in under `docs/tools/config-builder/vendor/` (or inline)
- [ ] The `<script src>` tag in `index.html` updated to point at the local copy
- [ ] The CDN fallback comment removed or updated to reflect the new approach
- [ ] Clicking **Validate** on a valid `profiles.json` shows the green "Valid" banner
- [ ] Clicking **Validate** on an invalid file shows specific error messages
- [ ] No regressions to other profile builder functionality

## Implementation Notes

Download the current Ajv v8 minified bundle:

```
https://cdn.jsdelivr.net/npm/ajv@8/dist/ajv.min.js
```

Save it to `docs/tools/config-builder/vendor/ajv.min.js` and update the `<script>` tag.
Do **not** use `npm` or a build step — the tool must remain a static HTML/JS page with no
build pipeline.

## Test Plan

1. Disconnect from the internet (or block the CDN in `/etc/hosts`).
2. Open `docs/tools/config-builder/index.html` locally.
3. Click **Validate** → verify the green "Valid" banner appears.
4. Introduce a deliberate schema error (e.g. remove a required field) → verify a specific
   error message appears.
5. Re-enable network and verify the page still works on GitHub Pages.
