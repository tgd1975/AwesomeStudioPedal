---
id: TASK-089
title: Cross-link simulator, profile builder, and configuration builder
status: closed
closed: 2026-04-17
opened: 2026-04-17
effort: Small (<2h)
complexity: Junior
human-in-loop: None
epic: Simulator
order: 4
---

## Description

The three web tools — simulator, profile builder, and configuration builder — currently have
only partial navigation between them. The simulator links to the profile builder; the profile
builder has no link back to the simulator; the configuration builder (TASK-088) will be new
and must be wired in from the start.

Every tool should have a consistent header nav so users can move between them without using
the browser's back button.

**Terminology reminder:**

- **Simulator** → `docs/simulator/`
- **Profile Builder** → `docs/tools/config-builder/` (builds `profiles.json`)
- **Configuration Builder** → `docs/tools/configuration-builder/` (builds `config.json`,
  created by TASK-088)

## Acceptance Criteria

- [ ] **Simulator** header contains links to both "Profile Builder →" and "Configuration Builder →"
- [ ] **Profile Builder** header contains links to "Simulator →" and "Configuration Builder →"
- [ ] **Configuration Builder** header contains links to "Simulator →" and "Profile Builder →"
- [ ] All links use correct relative paths (work on GitHub Pages and local `file://`)
- [ ] Link labels use the correct terminology (no "Config Builder" label — that term is retired)
- [ ] Existing `<a href="../tools/config-builder/">Config Builder →</a>` in the simulator
      is updated to "Profile Builder →" with the correct path

## Test Plan

1. Open each tool in a browser.
2. Click every cross-tool link and verify the correct destination loads.
3. Test on GitHub Pages and from local filesystem to confirm paths resolve in both contexts.

## Notes

Depends on TASK-088 (configuration builder must exist before its links can be added).
This task can be started on the simulator and profile builder sides independently, with the
configuration-builder links added once TASK-088 is done.
