---
id: TASK-078
title: Extend trigger system to handle button release events (non-blocking)
status: closed
closed: 2026-04-16
opened: 2026-04-16
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
---

## Description

The current trigger system only reacts to button-press events. `EventDispatcher` has a single
callback per button (`registerHandler`) and `Button::event()` only signals on press. The "high
while pressed" and "low while pressed" pin modes introduced in TASK-077 require the pedal to also
react on release — driving the pin back to its idle state the moment the user lifts the button.

All release handling must be non-blocking: no `delay()`, no spin-loops. The release event is
detected in the same polling loop in `loop()` that already processes press events.

Implementation steps:

1. **`IButton` interface** (`lib/PedalLogic/include/i_button.h`) — add a `releaseEvent()` pure
   virtual method that returns `true` exactly once per release edge, then resets.
2. **`Button` class** (`lib/hardware/esp32/include/button.h` and its `.cpp`) — the ISR already
   tracks `awaitingRelease`. Implement `releaseEvent()` to detect the HIGH→LOW→HIGH transition:
   when the pin returns HIGH after a press, set a `released` flag in the ISR and return+clear it
   from `releaseEvent()`.
3. **`EventDispatcher`** (`lib/PedalLogic/include/event_dispatcher.h`) — add
   `registerReleaseHandler(uint8_t button, EventCallback callback)` and
   `dispatchRelease(uint8_t button)`. Keep the existing press API unchanged.
4. **`main.cpp`** — in `process_events()`, call `actionButtonObjects[i]->releaseEvent()` for each
   button; if true, call `eventDispatcher.dispatchRelease(i)`.
5. **`setup_event_handlers()`** — for each button, register a release handler that calls
   `action->executeRelease()` (the no-op default from TASK-077 fires harmlessly for all
   non-pin actions).
6. The SELECT button does not need a release handler (profile switching is press-only).

## Acceptance Criteria

- [ ] `Button::releaseEvent()` fires exactly once per release edge; concurrent presses while
      the button is held do not generate spurious release events.
- [ ] `EventDispatcher::dispatchRelease()` calls the registered release callback, or does nothing
      if no callback is registered.
- [ ] `PinHighWhilePressed` pin goes LOW immediately on release; `PinLowWhilePressed` pin goes HIGH
      immediately on release.
- [ ] All existing press-only actions are unaffected (their `executeRelease()` is a no-op).
- [ ] Unit tests cover: press+release cycle for a pin-while-pressed action; release with no
      registered handler (no crash); release before press is ignored.

## Notes

Depends on TASK-077 for the `executeRelease()` virtual method on `Action`.

The `Button` ISR (`isr()`) already uses `awaitingRelease` to guard against bouncing on the
release edge — reuse that flag rather than reading `digitalRead()` from `releaseEvent()`, which
would be ISR-unsafe.

The nrf52840 hardware port (`lib/hardware/nrf52840/include/button.h`) must also implement
`releaseEvent()` to keep the build consistent, even if it is a stub initially.
