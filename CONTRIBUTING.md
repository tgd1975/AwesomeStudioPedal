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

## Code Quality Standards

- **Zero Warnings**: All compiler warnings are errors (`-Werror`). Fix before committing.
- **Static Analysis**: Code must pass `clang-tidy` — run locally with `clang-tidy -p .vscode/build <file>`
- **Formatting**: Run `clang-format` before committing — the pre-commit hook enforces this automatically.
- **No Code Smells**: New code must not introduce magic numbers, deep nesting, or long functions.
- **Test Coverage**: New behaviour must have a corresponding host unit test.

## Quick Checklist

1. Fork the repository.
2. Branch off `main` using the naming convention above.
3. `make test-host` passes with zero failures.
4. `clang-tidy` produces no errors on changed files.
5. Open a pull request — the PR template will guide you.
6. Squash-merge after review.

## Task Workflow

Open tasks are tracked in [docs/developers/tasks/](docs/developers/tasks/).

- **Browse tasks**: see [OVERVIEW.md](docs/developers/tasks/OVERVIEW.md) or look in `open/`
- **Pick up a task**: read its file, branch off `main` (`feature/TASK-NNN-short-title`)
- **Close a task**: use `/task-done TASK-NNN` — moves the file to `closed/` and updates the overview
- **Create a task**: use `/task-new "Short title"` — scaffolds the file and updates the overview
- **Update overview manually**: `python scripts/update_task_overview.py`

Each task file contains a description, acceptance criteria, and notes.

## Further Reading

- [BRANCH_PROTECTION_CONCEPT.md](docs/developers/BRANCH_PROTECTION_CONCEPT.md)
- [CODING_STANDARDS.md](docs/developers/CODING_STANDARDS.md)
- [CODE_OF_CONDUCT.md](docs/developers/CODE_OF_CONDUCT.md)
