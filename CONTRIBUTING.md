# Contributing

Read the developer docs before submitting a pull request — they cover architecture, testing, and
coding standards.

## Branch Strategy

- `main` is the protected stable branch — no direct pushes
- Branch off `main` using the naming convention below
- Open a pull request; CI must pass and one review is required before merge
- Squash-merge after review

### Branch Naming

| Prefix | Purpose | Example |
|--------|---------|---------|
| `feature/` | New functionality | `feature/profile-led-blinking` |
| `fix/` | Bug fixes | `fix/profile-select-button` |
| `docs/` | Documentation only | `docs/update-contributing` |
| `chore/` | Maintenance, CI, tooling | `chore/add-dependabot` |

### Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <short description>
```

Types: `feat`, `fix`, `docs`, `chore`, `test`, `refactor`

## Quick Checklist

1. Fork the repository.
2. Branch off `main` using the naming convention above.
3. `make test-host` passes with zero failures.
4. Open a pull request — the PR template will guide you.
5. Squash-merge after review.

## Further Reading

- [BRANCH_PROTECTION_CONCEPT.md](docs/developers/BRANCH_PROTECTION_CONCEPT.md)
- [CODING_STANDARDS.md](docs/developers/CODING_STANDARDS.md)
- [CODE_OF_CONDUCT.md](docs/developers/CODE_OF_CONDUCT.md)
