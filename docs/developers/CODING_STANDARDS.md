# Coding Standards

## C++ standard

C++14. This is required for PlatformIO/Arduino ecosystem compatibility.

## Code formatting

clang-format is used for all C++ files. Run:

```bash
./scripts/format-code.sh
```

The pre-commit hook enforces formatting automatically on staged files.

## Naming conventions

| Item | Convention | Example |
|------|------------|---------|
| Classes | PascalCase | `ProfileManager` |
| Methods and variables | camelCase | `switchProfile()`, `currentIndex` |
| Constants and macros | UPPER_SNAKE_CASE | `NUM_PROFILES`, `GPIO_LED_BT` |

## Commit message format

```
type: short description
```

Types: `feat`, `fix`, `refactor`, `docs`, `chore`. One logical change per commit.

## Branch naming

Patterns: `feature/`, `fix/`, `refactor/`, `docs/` — lowercase, hyphens only, four words or fewer.
Reference an issue number when one exists: `fix/42-watchdog-reset`.

## Branch lifecycle

1. Branch off `main`.
2. Keep branches short-lived — open a PR within a few days.
3. One concern per branch.
4. All tests must pass before requesting review (`make test-host`).
5. Squash-merge into `main`.
6. Delete the branch after merge.

No long-lived branches.

## `main` branch protection

No direct pushes to `main`. Every merge requires a pull request. `make test-host` must pass locally
before pushing.

## Semantic versioning

| Segment | Increment when |
|---------|---------------|
| `MAJOR` | Breaking change to the hardware interface or config format |
| `MINOR` | New feature: new platform, new action type |
| `PATCH` | Bug fix or documentation update |

## Release checklist

1. `make test-host` passes on `main` with zero failures.
2. `make build` completes cleanly for all target environments.
3. Bump version in `platformio.ini`.
4. Tag on `main`:

   ```bash
   git tag -a vX.Y.Z -m "Release vX.Y.Z"
   git push origin vX.Y.Z
   ```

5. Create a GitHub Release from the tag; attach `.elf`/`.bin` artefacts for each environment.
