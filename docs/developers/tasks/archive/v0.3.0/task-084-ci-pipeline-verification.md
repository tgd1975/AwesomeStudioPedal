---
id: TASK-084
title: CI pipeline verification
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Medium
human-in-loop: Support
epic: Finalization
order: 6
---

## Description

Verify that all GitHub Actions workflows pass cleanly on the `main` branch before the first
public release. Check `test.yml`, `docs.yml`, `static-analysis.yml`, `codeql-analysis.yml`,
and the release workflow. Confirm that GitHub Pages deploys correctly (simulator and config
builder accessible at their published URLs), that test badges in `README.md` reflect real
pass status, and that no workflow is silently skipped or misconfigured.

## Acceptance Criteria

- [ ] All workflows (`test`, `docs`, `static-analysis`, `codeql-analysis`) show green on the
      `main` branch in GitHub Actions
- [ ] GitHub Pages deployment succeeds and both
      `https://tgd1975.github.io/AwesomeStudioPedal/simulator/` and
      `https://tgd1975.github.io/AwesomeStudioPedal/tools/config-builder/` load correctly
- [ ] README.md badges accurately reflect the current CI status (no stale badge URLs or
      pointing to non-existent workflows)

## Test Plan

No automated tests required — verification is done by inspecting GitHub Actions run results
and the live Pages URLs in a browser.

## Notes

- Use `/ci-status` to get a quick summary of recent workflow runs without leaving the CLI.
- The `docs.yml` workflow was updated in TASK-031 (simulator deploy) and TASK-032 (config
  builder deploy); verify both copy steps execute and the Pages artifact contains both tools.
- The release workflow has never been triggered yet (no tags exist); confirm it is syntactically
  valid by reviewing it, but do not trigger it here — that is the `/release` task.
- If any workflow is failing on `main`, fix it before cutting the first release tag.
