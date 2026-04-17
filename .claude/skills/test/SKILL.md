---
name: test
description: Run host unit tests via make test-host and report results
---

# test

Run the host unit test suite and report a pass/fail summary.

Steps:

1. Run `make test-host` (equivalent to `cmake --build .vscode/build --target pedal_tests &&
   .vscode/build/test/pedal_tests`).
2. Capture stdout and stderr.
3. Report:
   - Total tests run, passed, failed
   - Any failing test names with their failure messages
   - Overall status: PASS or FAIL

If the build step fails before tests run, report the build error and stop.

Do not suggest fixes — just report what happened.

## When to run

Run `/test` after every implementation change that touches `lib/PedalLogic/` or `test/unit/`.
Hardware-dependent code that cannot be shimmed requires on-device validation — use `/test-device`
for that.
