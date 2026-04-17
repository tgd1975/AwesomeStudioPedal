---
name: test-device
description: Run on-device (hardware) tests via PlatformIO and report results. Requires a physical device connected via USB.
---

# test-device

Run on-device integration tests for a specific target and report pass/fail.

The user invokes this as `/test-device <target>` where `<target>` is one of:

| Target | Make command | Device needed |
|---|---|---|
| `esp32-button` | `make test-esp32-button` | ESP32 via USB |
| `esp32-serial` | `make test-esp32-serial` | ESP32 via USB |
| `esp32-profiles` | `make test-esp32-profiles` | ESP32 via USB |
| `nrf52840-button` | `make test-nrf52840-button` | nRF52840 via USB |
| `nrf52840-serial` | `make test-nrf52840-serial` | nRF52840 via USB |
| `nrf52840-profiles` | `make test-nrf52840-profiles` | nRF52840 via USB |

Steps:

1. If no target is given, list the available targets above and stop — ask the user which to run.
2. Map the target to its `make` command from the table above. If the target is not in the table,
   report "Unknown target" and list valid options.
3. Run the make command and capture stdout + stderr.
4. Report:
   - Which tests ran and their pass/fail status (Unity output format: `X Tests Y Failures Z Ignored`)
   - Any test failure details (test name + failure message)
   - Overall status: PASS or FAIL
5. If the build fails before tests run (e.g. compilation error or no device found), report the
   error and stop. A "no device found" error means the USB device is not connected — tell the user.

Do not suggest fixes — just report what happened.

## When to run

Run `/test-device` for code that touches:

- GPIO / pin state (`digitalRead`, `digitalWrite`, `pinMode`)
- Interrupt attachment (`attachInterrupt`, ISR functions)
- Hardware button debounce timing (real-time edge detection)
- Serial output format on device boot
- BLE connection behaviour

If the change only touches `lib/PedalLogic/` logic with no hardware calls, use `/test` (host)
instead — it is faster and requires no device.
