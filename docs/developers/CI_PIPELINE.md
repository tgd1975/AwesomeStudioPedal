# CI Pipeline

## Pre-commit hook

The pre-commit hook runs `make test-host` before every commit. If host tests fail, the commit is
blocked. Do not bypass this with `--no-verify`.

It also runs markdownlint on staged `.md` files.

## GitHub Actions — CI

File: `.github/workflows/test.yml`

Triggers on every push and on pull requests to `main`.

| Job | What it does |
|-----|-------------|
| `cpp-format` | Runs clang-format check across C++ sources |
| `markdown-lint` | Runs markdownlint-cli2 on root `*.md` files |
| `unit-tests` | CMake build + GoogleTest run + publishes test results |

All jobs must pass before a PR can be merged.

## GitHub Actions — Docs

File: `.github/workflows/docs.yml`

Triggers on push to `main` and on manual dispatch.

| Job | What it does |
|-----|-------------|
| `build-docs` | Installs Doxygen, runs `doxygen Doxyfile`, uploads Pages artifact |
| `deploy-docs` | Deploys the artifact to GitHub Pages |

**Prerequisite:** GitHub repo → Settings → Pages → Source must be set to **GitHub Actions**.
This is a one-time manual step in the repository settings.

## Doxygen configuration

The `Doxyfile` at the repository root configures Doxygen. Input directories:

- `src/`
- `include/`
- `lib/PedalLogic/`
- `lib/hardware/esp32/`
- `lib/hardware/nrf52840/`

Output: `docs/api/html/`. The `docs/api/` directory is gitignored — generated output is never
committed.

## Release process

1. All tests pass on `main`.
2. Bump version in `platformio.ini`.
3. Tag the release:

   ```bash
   git tag -a vX.Y.Z -m "Release vX.Y.Z"
   git push origin vX.Y.Z
   ```

4. Create a GitHub Release from the tag; attach `.elf`/`.bin` artefacts for each target environment.
