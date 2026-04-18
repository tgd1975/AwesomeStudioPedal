---
id: TASK-131
title: Builders Docs — CLI Tool Usage
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: No
group: MobileApp
order: 14
prerequisites: [TASK-144]
---

## Description

Document `scripts/pedal_config.py` for builders in `docs/builders/CLI_TOOL.md`. The doc should be self-contained — a builder with no prior knowledge of the tool should be able to install, connect, and upload a profile after reading it.

## Acceptance Criteria

- [ ] `docs/builders/CLI_TOOL.md` created
- [ ] Section 1: **Prerequisites** — Python 3.9+, `pip install -r scripts/requirements.txt`, BLE adapter requirement
- [ ] Section 2: **Scanning** — `python scripts/pedal_config.py scan` with expected output example
- [ ] Section 3: **Uploading profiles** — `python scripts/pedal_config.py upload profiles.json` step-by-step with expected terminal output and LED confirmation description
- [ ] Section 4: **Validating without upload** — `python scripts/pedal_config.py validate profiles.json` with schema error interpretation guide
- [ ] Section 5: **Uploading hardware config** — `python scripts/pedal_config.py upload-config config.json`
- [ ] Section 6: **Troubleshooting** — device not found, permission denied, upload timeout — each with a concrete fix
- [ ] `docs/builders/` index or nav links updated to reference the new doc

## Files to Touch

- `docs/builders/CLI_TOOL.md` (new)
- `docs/builders/` index file (if exists)

## Test Plan

Review only — a second pair of eyes should follow the doc on a fresh machine and confirm it works end-to-end.

## Prerequisites

- **TASK-144** — all configurator screens (including JSON preview) must be final before documenting the flow
