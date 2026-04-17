---
id: TASK-021
title: Add package publishing
status: closed
closed: 2026-04-16
opened: 2026-04-10
effort: Large (8-24h)
complexity: Senior
human-in-loop: Clarification
---

## Description

Evaluate whether firmware binaries should be published as packages (e.g. via PlatformIO registry,
GitHub Packages, or similar) and implement publishing if applicable.
This is a follow-on to TASK-020 (release workflow).

## Acceptance Criteria

- [x] Decision made on which package registry to use (or explicit decision to skip)
- [x] Decision and rationale documented in `## Notes`

## Notes

**Decision: Skip package publishing for now.**

Rationale:

- The PlatformIO registry is designed for libraries, not firmware binaries — it is not the
  right vehicle for distributing `.bin` files to end users.
- GitHub Packages supports container images and language packages (npm, Maven, etc.),
  not bare firmware binaries.
- GitHub Releases (implemented in TASK-020) already provides versioned, tagged downloads
  with direct URLs, which is exactly what a hobbyist/musician user needs.
- Adding a registry step would increase CI complexity with no tangible user benefit at this
  project's current scale.

**Revisit if:** the project gains a programmatic consumer that needs dependency-resolution
(e.g. a companion app that auto-updates the pedal firmware), at which point GitHub Packages
OCI or a custom update manifest would be the right tool.
