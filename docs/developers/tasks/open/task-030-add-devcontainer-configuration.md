---
id: TASK-030
title: Add .devcontainer configuration
status: open
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Add a `.devcontainer/devcontainer.json` configuration so contributors can open the project
in a fully configured VS Code Dev Container or GitHub Codespaces with one click.

## Acceptance Criteria

- [ ] `.devcontainer/devcontainer.json` exists
- [ ] Container includes all required build tools (PlatformIO, CMake, clang-format, clang-tidy, Python)
- [ ] `make test-host` runs successfully inside the container
- [ ] VS Code extensions for C++, PlatformIO, and markdownlint are pre-installed
- [ ] README mentions dev container as a setup option

## Notes

Goal: one-click development environment setup for new contributors.
The container should mirror the CI environment as closely as possible to avoid "works on my machine" issues.
