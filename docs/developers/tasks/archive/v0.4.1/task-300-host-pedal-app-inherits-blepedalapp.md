---
id: TASK-300
title: Phase 4e — HostPedalApp inherits BlePedalApp; accepts injected dependencies
status: closed
opened: 2026-04-28
closed: 2026-04-28
effort: Small (<2h)
effort_actual: XS (<30m)
complexity: Medium
human-in-loop: Support
epic: hal-refactor
order: 12
prerequisites: [TASK-299]
---

## Description

Make `HostPedalApp` actually usable for end-to-end unit tests of the
pedal application loop. Today it inherits from `PedalApp` directly
and skips the BLE layer entirely — that means tests can construct
it but cannot call `setup()` / `loop()` and assert on the resulting
behaviour (no `loop()` body exists at the `PedalApp` level).

After this task, `HostPedalApp` mirrors the on-device subclasses:

```cpp
class HostPedalApp : public BlePedalApp
{
public:
    explicit HostPedalApp(IBleKeyboard* bleKeyboard);

protected:
    void platformSetup() override {}
    void platformLoop()  override {}
    void saveProfile(uint8_t index) override;
    uint8_t loadProfile() override;

private:
    uint8_t storedProfile_ = 0;   // in-memory, no-NVS persistence
};
```

Construction shape from a test:

```cpp
auto* mockKb = new MockBleKeyboard();
HostPedalApp app(mockKb);
app.setup();
// drive button input via fake_gpio + advance fake_time;
// app.loop() runs the real BlePedalApp body;
// assert on mockKb's recorded write() calls.
```

`saveProfile` / `loadProfile` use a tiny in-memory member — enough
to round-trip a profile index between test calls without dragging
in `IFileSystem` (TASK-302 may revisit if needed).

## Acceptance Criteria

- [ ] `src/host/include/host_pedal_app.h` declares
      `class HostPedalApp : public BlePedalApp`.
- [ ] Constructor takes `IBleKeyboard*` and forwards to
      `BlePedalApp(bleKeyboard)`.
- [ ] `platformSetup` / `platformLoop` are no-ops; `saveProfile`
      stores in a private `uint8_t`; `loadProfile` returns it
      (default 0).
- [ ] If a `.cpp` is needed (because BlePedalApp's constructor isn't
      `inline`), add `src/host/src/host_pedal_app.cpp` and add it
      to `PROD_SOURCES` in `test/CMakeLists.txt`.
- [ ] `test/unit/test_host_pedal_app.cpp` is updated: smoke test
      now constructs `HostPedalApp` with a `MockBleKeyboard`
      from `test/mocks/`. The test still just verifies
      construction succeeds (richer scenarios come in TASK-302).
- [ ] `make test-host` passes (287 tests).

## Test Plan

**Host tests** (`make test-host`):

- The 287-test suite still passes.
- Updated smoke test constructs `HostPedalApp(mockKb)` instead
  of the no-arg form.

## Prerequisites

- **TASK-299** — `HostPedalApp` lives at `src/host/include/`.

## Notes

- This task does NOT yet add per-pin `fake_gpio` (TASK-301) or
  end-to-end tests (TASK-302). It just makes `HostPedalApp`
  capable of running `BlePedalApp::loop()`.
- The `BlePedalApp` global ISR pointer (`g_blePedalApp`) gets
  set when a `HostPedalApp` is constructed. Tests need to be
  aware that creating two `HostPedalApp` instances in the same
  process is unsupported (matches the on-device reality — there
  is exactly one pedal). `TEST_F` fixtures should construct one
  per test method, destruct in teardown.
