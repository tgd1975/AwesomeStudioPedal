# Contributing

Read the developer docs before submitting a pull request — they cover architecture, testing, and
coding standards.

## How this project is developed

AwesomeStudioPedal is a one-person project. The owner acts as product architect — defining use
cases, architecture, and requirements — and wrote the original prototype. Ongoing development
is heavily AI-assisted: Claude Code and similar tools write most of the code, while the owner
reviews, directs, and accepts or rejects the result. This is not hidden.

The quality floor is maintained by the pre-commit hooks, CI pipeline, clang-format, clang-tidy,
and test coverage requirements — not by who or what produced the code.

### What this means for your contribution

Contributions from humans, AI tools, or human-AI pairs are equally welcome, subject to one
standard: **the PR must have a clear, stated purpose that the maintainer can evaluate**.

A PR without a clear explanation of what it accomplishes and why will not be accepted —
regardless of code quality. *"I used an AI tool to generate this"* is not a reason to reject.
*"I cannot tell what this is trying to do"* is.

The simplest path to a merged PR:

1. Open an issue first — describe what you want to accomplish and why.
2. Reference that issue in the PR.
3. Pass CI.

If the maintainer cannot understand the purpose of a PR, it will not be merged.

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

When a commit relates to a tracked task, include the task ID in the scope:

```
feat(TASK-047): fix profile LED blinking after profile switch
fix(TASK-054): resolve license inconsistency in package manifest
```

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

## Claude Code Skills

This project ships custom Claude Code skills in `.claude/skills/`. Invoke them with a `/` prefix
inside a Claude Code session:

| Skill | What it does |
|-------|-------------|
| `/tasks` | List all open tasks in a compact table |
| `/ts-task-new "Title"` | Scaffold a new task file and update OVERVIEW.md |
| `/ts-task-done TASK-NNN` | Close a task and update OVERVIEW.md |
| `/test` | Run host unit tests (`make test-host`) and report results |
| `/lint` | Run markdown lint, code smell check, and Mermaid validation |
| `/clang-tidy` | Run clang-tidy on C++ files changed relative to `main` |
| `/format` | Auto-format all C++ files with clang-format and show what changed |
| `/fix-markdown` | Auto-fix markdown lint issues and show what changed |
| `/ci-status` | Summarize recent GitHub Actions run statuses per workflow |
| `/doc-check` | Validate documentation persona placement |
| `/release vX.Y.Z` | Bump version, create annotated tag, and push |

Each skill is documented in `.claude/skills/<name>/SKILL.md`.

## Task Workflow

Open tasks are tracked in [docs/developers/tasks/](docs/developers/tasks/).

- **Browse tasks**: see [OVERVIEW.md](docs/developers/tasks/OVERVIEW.md) or run `/tasks`
- **Pick up a task**: read its file, branch off `main` (`feature/TASK-NNN-short-title`)
- **Close a task**: use `/ts-task-done TASK-NNN` — moves the file to `closed/` and updates the overview
- **Create a task**: use `/ts-task-new "Short title"` — scaffolds the file and updates the overview
- **Update overview manually**: `python scripts/update_task_overview.py`

Each task file contains a description, acceptance criteria, and notes.

## Release Process

Maintainers only. All steps run on `main` from a clean working tree.

### 1 — Bump the version

Use the `/release vX.Y.Z` skill inside a Claude Code session. It:

1. Verifies the working tree is clean and the branch is `main`.
2. Bumps `version =` in `platformio.ini` **and** `FIRMWARE_VERSION` in `include/version.h`.
3. Commits the bump (`chore: bump version to vX.Y.Z`).
4. Creates an annotated tag (`git tag -a vX.Y.Z -m "Release vX.Y.Z"`).
5. Asks for confirmation, then pushes the commit and tag.

### 2 — CI builds the release

Pushing the `vX.Y.Z` tag triggers `.github/workflows/release.yml`, which:

- Runs all host unit tests.
- Builds firmware for ESP32 (`nodemcu-32s`) and nRF52840 (`feather-nrf52840`).
- Creates a GitHub Release and uploads the `.bin` files as assets.

Monitor progress on the **Actions** tab. The release is live once the workflow succeeds.

### 3 — Post-release cleanup

Run the cleanup script to keep only the current and two previous releases:

```bash
bash scripts/cleanup-releases.sh
```

The script uses `gh release delete --yes --cleanup-tag` and is idempotent.
See [`scripts/cleanup-releases.sh`](scripts/cleanup-releases.sh) for details.

### 4 — Update firmware links

After the release is live, update `README.md` with the new download links.
Use `gh release view vX.Y.Z --json assets` to get the exact asset URLs.

### Versioning convention

Releases follow [Semantic Versioning](https://semver.org/): `vMAJOR.MINOR.PATCH`.
Use Conventional Commits to determine the bump type — `feat` → minor, `fix` → patch,
breaking change → major.

## Further Reading

- [DEVELOPMENT_SETUP.md](docs/developers/DEVELOPMENT_SETUP.md) — required tools, build commands, host test setup
- [BRANCH_PROTECTION_CONCEPT.md](docs/developers/BRANCH_PROTECTION_CONCEPT.md)
- [CODING_STANDARDS.md](docs/developers/CODING_STANDARDS.md)
- [CODE_OF_CONDUCT.md](docs/developers/CODE_OF_CONDUCT.md)
