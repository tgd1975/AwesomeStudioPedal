# Branch Protection Concept

## Overview

This document describes the branch protection strategy for the AwesomeStudioPedal repository.

## Branch Strategy

```
main
 └── feature/* or fix/*  (short-lived, opened as PRs)
```

- `main` is the single protected branch
- All changes arrive via pull request
- Direct pushes to `main` are not permitted (except repository maintainers in emergencies)

## Protection Rules

| Rule | Setting |
|------|---------|
| Require status checks to pass | Yes — CI job must pass |
| Require branches to be up to date | Yes |
| Require pull request review | 1 approving review |
| Dismiss stale reviews on new push | Yes |
| Enforce for administrators | Yes |
| Allow force pushes | No |
| Allow deletions | No |

## Implementation

Branch protection is configured via `.github/settings.yml` (requires the
[Settings GitHub App](https://github.com/apps/settings) to be installed):

```yaml
repository:
  branch_protection_rules:
    - pattern: main
      required_status_checks:
        strict: true
        contexts: ["CI"]
      enforce_admins: true
      required_pull_request_reviews:
        required_approving_review_count: 1
        dismiss_stale_reviews: true
      allow_force_pushes: false
      allow_deletions: false
```

Alternatively, configure directly in the GitHub UI under
**Settings → Branches → Branch protection rules**.

## Branch Naming Conventions

| Prefix | Purpose | Example |
|--------|---------|---------|
| `feature/` | New functionality | `feature/profile-led-blinking` |
| `fix/` | Bug fixes | `fix/profile-select-button` |
| `docs/` | Documentation only | `docs/update-contributing` |
| `chore/` | Maintenance, CI, tooling | `chore/add-dependabot` |

## Commit Message Convention

Follow the [Conventional Commits](https://www.conventionalcommits.org/) format:

```
<type>(<scope>): <short description>

[optional body]
```

Types: `feat`, `fix`, `docs`, `chore`, `test`, `refactor`

## Rationale

- Prevents accidental direct pushes to `main`
- Ensures CI passes before any code lands
- Provides a lightweight review gate for a solo/small-team project
- Keeps `main` always in a releasable state
