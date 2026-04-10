---
id: TASK-005
title: Add issue and PR templates
status: closed
opened: 2026-04-10
closed: 2026-04-10
effort: Medium (2-8h)
complexity: Junior
human-in-loop: Clarification
---

## Description

Add structured GitHub issue templates (bug report, feature request) and a pull request template
to guide contributors in providing the right information.

## Acceptance Criteria

- [x] `.github/ISSUE_TEMPLATE/bug_report.md` created
- [x] `.github/ISSUE_TEMPLATE/feature_request.md` created
- [x] `.github/pull_request_template.md` created

## Notes

Delivered in commit `0c2febc`.
Bug report template: title prefix `[Bug]`, sections for reproduction steps, expected/actual behavior, environment.
Feature request template: title prefix `[Feature]`, sections for problem statement, proposed solution, alternatives.
PR template: Description, Related Issue, Changes Made, Checklist.
