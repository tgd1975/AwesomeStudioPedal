---
id: IDEA-046
title: Investigate the ble_config_service.cpp copy loop — possible build-system smell
status: open
opened: 2026-04-28
---

# Investigate the ble_config_service.cpp copy loop

In chat session `7f6bff0f-287d-4a16-b1fa-2bef94b998a5` the agent ran
`cp /home/tobias/Dokumente/Projekte/AwesomeStudioPedal/lib/hardware/esp32/src/ble_config_service.cpp \ …`
**16 times** alongside 7 invocations of
`python3 test/test_ble_config_esp32/runner.py --port /dev/ttyUSB0`.

This pattern — repeatedly copying a single source file between locations
in tight rotation with on-device test runs — strongly suggests one of:

- The on-device test build is not picking up edits to the canonical
  source automatically (missing include path, wrong source root in
  `platformio.ini`).
- The agent misunderstood where the test harness reads the source from
  and is keeping two copies in sync by hand.
- A symlink that should exist is a real file (or vice versa).
- The test harness has its own private copy under `test/` that drifted.

The fix could be small (one `platformio.ini` line, one symlink) or
larger (restructure the test build). It is not safe to scaffold a task
until the root cause is understood.

## Open questions

- What were the source and destination paths of the `cp`? (Need to read
  the session.)
- Did the test runner output change after the copy, or was the copy a
  workaround for something else?
- Does the issue still reproduce on the current branch, or has the HAL
  refactor (EPIC-020) already addressed it?

## Next step

Read the session transcript at
`~/.claude/projects/-home-tobias-Dokumente-Projekte-AwesomeStudioPedal/7f6bff0f-287d-4a16-b1fa-2bef94b998a5.jsonl`
and report the actual paths and the test outcome. Promote to a task only
after the root cause is identified.

## Notes

- Source signal: 16× `cp ble_config_service.cpp` in one session.
- Filed as an idea per the "uncertainty → idea" rule
  (memory: `feedback_idea_vs_task.md`) — root cause is unknown, so
  acceptance criteria cannot yet be written.
