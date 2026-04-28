---
id: TASK-168
title: Write generate-profiles-index script and validate-profiles npm command
status: closed
closed: 2026-04-19
opened: 2026-04-19
effort: Small (<2h)
effort_actual: XS (<30m)
effort_actual_source: heuristic-v1
complexity: Medium
human-in-loop: No
epic: CommunityProfiles
order: 3
---

## Description

Write `scripts/generate-profiles-index.js` — a Node script that walks
`profiles/**/*.json`, reads each file's `_meta` block plus the `profiles` array, and
writes `profiles/index.json` matching the schema in IDEA-017. Also add two npm scripts
to the root `package.json`.

The script derives `minButtons` from the subfolder name (e.g. `2-button/` → 2), reads
`_meta.id`, `_meta.author`, `_meta.tags`, `_meta.tested`, `_meta.minFirmware` (optional),
counts `profiles[]` for `profileCount`, and collects `profiles[].name` for the `profiles`
names list.

## Acceptance Criteria

- [ ] `scripts/generate-profiles-index.js` exists and runs with `node scripts/generate-profiles-index.js`
- [ ] Output `profiles/index.json` matches the structure in IDEA-017 (version, generated timestamp, profiles array with all specified fields)
- [ ] `minFirmware` included in index entry when present in `_meta`, omitted when absent
- [ ] `npm run generate-profiles-index` added to root `package.json`
- [ ] `npm run validate-profiles` added to root `package.json` — runs ajv against `data/profiles.schema.json` for every `.json` file under `profiles/` (excluding `index.json`)
- [ ] Running the script on the 12 starter profiles (TASK-167) produces a valid, non-empty `index.json`

## Notes

The ajv toolchain is already available in the config builder (`docs/tools/config-builder/vendor/ajv.min.js`). Check if a Node-compatible ajv is already in `package.json` before adding a new dependency.

Depends on: TASK-166 (folder structure), TASK-167 (at least some profiles to test against).
