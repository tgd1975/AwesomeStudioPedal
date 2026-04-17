---
id: TASK-113
title: Wire MacroAction::update in main.cpp
status: open
opened: 2026-04-17
effort: Trivial (<30m)
complexity: Junior
human-in-loop: No
group: Macros
order: 4
prerequisites: [TASK-112]
---

## Description

Call `MacroAction::update()` from the main loop so that multi-step macros advance between loop ticks. Without this call, a macro fires step 0 on `execute()` but never advances to subsequent steps.

## Acceptance Criteria

- [ ] `loop()` iterates over all buttons and calls `update()` on any `MacroAction` found
- [ ] Firmware compiles and runs without warning on ESP32 target
- [ ] A two-step macro configured in `profiles.json` fires both steps in order during manual testing

## Files to Touch

- `src/main.cpp` — add update loop after the existing `DelayedAction` update call

## Test Plan

Manual smoke test: configure a two-step macro (e.g. `PinHigh` → `SendKey`) and verify both steps fire. **Host tests** covered by TASK-114.

## Prerequisites

- **TASK-112** — config loader must parse macros before `main.cpp` can receive a `MacroAction` instance

## Notes

Two implementation options:

**Option A (dynamic_cast):**
```cpp
for (uint8_t i = 0; i < hardwareConfig.numButtons; i++) {
    if (auto* macro = dynamic_cast<MacroAction*>(
            profileManager->getAction(profileManager->getCurrentProfile(), i))) {
        macro->update();
    }
}
```

**Option B (virtual no-op on base):** Add `virtual void update() {}` to `Action` base class; call `action->update()` for all actions each tick. Eliminates the cast overhead. Preferred if cast overhead is a concern on the embedded target.

Choose Option B if touching `action.h` is acceptable in this task.
