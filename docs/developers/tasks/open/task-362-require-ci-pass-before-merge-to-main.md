---
id: TASK-362
title: Require all CI checks to pass before merging to main
status: open
opened: 2026-05-02
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Support
---

## Description

Today nothing on GitHub prevents a PR from being merged into `main` while CI is
red. Several recent tasks (TASK-321 "Fix CI failures and prevent dirty pushes",
TASK-352 "Investigate pre-commit hook latency") imply we already treat green CI
as a gate, but it is enforced only socially — there is no branch-protection
rule on `main` that requires the workflow checks to pass.

This task closes that gap: configure GitHub branch protection on `main` so that
every CI workflow status check must pass (and the branch must be up-to-date
with `main`) before the PR can be merged. It also covers updating the local
skills and developer docs that talk about merging so the rule is discoverable
and the contributor onboarding stays consistent.

Scope:

1. **Audit** `.github/workflows/` and list every workflow + the status-check
   names they publish. Decide which ones are mandatory gates (likely all of
   them — markdown lint, code smells, mermaid lint, host tests, on-device-eligible
   builds, clang-tidy, sync_task_system check).
2. **Configure** branch protection on `main` via the GitHub UI or
   `gh api -X PUT repos/:owner/:repo/branches/main/protection ...`. Include:
   - Require pull request before merging.
   - Require status checks to pass before merging — list the checks from step 1.
   - Require branches to be up to date before merging.
   - Do not allow bypasses for admins (or document the exception).
3. **Document** the gate and its required-checks list in
   `docs/developers/DEVELOPMENT_SETUP.md` (or a new `CI.md` if that page gets
   too crowded), and add a short "How to add a new required check" recipe so
   future workflow additions don't silently slip through ungated.
4. **Update skills/docs** that talk about merging or commits so they reference
   the gate: `/release-branch` (its merge step), `/commit` skill body and
   `COMMIT_POLICY.md` (cross-link the gate as the upstream enforcement layer).

## Acceptance Criteria

- [ ] Branch-protection rule on `main` requires every CI workflow status check
      to pass before a PR is mergeable, and requires the branch to be
      up-to-date with `main`.
- [ ] The required-checks list is documented in
      `docs/developers/DEVELOPMENT_SETUP.md` and matches the checks actually
      configured on GitHub.
- [ ] A short "How to add a new required check" section exists in the same
      doc, explaining the procedure (add workflow → push → wait for first run
      to register the check name → add to branch protection).
- [ ] `/release-branch`, `/commit` skill body, and
      `docs/developers/COMMIT_POLICY.md` reference the merge gate so the
      enforcement story is consistent across docs.
- [ ] Manual verification: a throwaway PR with one deliberately failing
      workflow check cannot be merged via the GitHub UI.

## Test Plan

No automated tests — change is configuration + documentation, not C++ logic.

**Manual verification:**

- Open a throwaway PR that introduces a deliberately failing check (e.g.
  a markdownlint violation in a `.md` file or an obvious clang-tidy warning).
- Wait for CI to report the failure.
- Confirm the GitHub "Merge pull request" button is disabled / blocked with
  the "required statuses must pass" message.
- Fix the failure, push, wait for green, confirm merge becomes available
  again. Close the PR without merging.

## Documentation

- `docs/developers/DEVELOPMENT_SETUP.md` — add a "CI merge gate" section:
  required-checks list and "How to add a new required check" recipe.
- `docs/developers/COMMIT_POLICY.md` — cross-reference the merge gate as the
  upstream enforcement layer that complements the local pre-commit hook.
- `.claude/skills/release-branch/SKILL.md` — if its merge step assumes the
  agent can merge unconditionally, update it to acknowledge the gate (and to
  fail loudly if checks are red rather than waiting silently).
- `.claude/skills/commit/SKILL.md` — short note that local /commit success
  does not imply mergeability; PR still has to clear the CI gate.

## Notes

- Configuring branch protection requires admin permission on the GitHub repo.
  If using `gh api`, the token needs the `repo` scope (or fine-grained
  equivalent). User confirmation will be required before the agent runs the
  `gh api` call — this is exactly the kind of "shared-state, hard-to-reverse"
  action that should not be taken silently.
- Required-check names on GitHub are derived from the `name:` field of each
  job in the workflow YAML. Renaming a job silently breaks branch protection
  (the old name stays required, the new name isn't). Worth calling out in the
  "How to add a new required check" doc.
- Consider whether to also require **signed commits** and **linear history**
  while we're configuring branch protection. Out of scope for this task
  unless the user opts in — flag it in the PR description as a follow-up.
- Related: TASK-321 fixed the CI failures themselves; this task fixes the
  *gate*. TASK-361 (lint platformio.ini in pre-commit and CI) will add a new
  CI check that should be added to the required list as part of its own
  rollout — once both land, document the dependency in TASK-361 too.
