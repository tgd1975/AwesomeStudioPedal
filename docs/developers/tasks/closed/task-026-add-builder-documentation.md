---
id: TASK-026
title: Add builder documentation
status: closed
closed: 2026-04-16
opened: 2026-04-10
effort: Medium (2-8h)
complexity: Medium
human-in-loop: Clarification
---

## Description

Create `docs/building.md` covering everything a builder needs: how to compile the firmware,
configure the hardware, and prepare a device for end-users.

## Acceptance Criteria

- [ ] `docs/building.md` exists
- [ ] Covers build prerequisites (PlatformIO, CMake, platform SDKs)
- [ ] Step-by-step build instructions for ESP32 and nRF52840
- [ ] References upload instructions (TASK-027) and required tools (TASK-028)
- [ ] Linked from `README.md`

## Notes

Related: TASK-027 (upload instructions), TASK-028 (required tools), TASK-025 (firmware versions in README).
