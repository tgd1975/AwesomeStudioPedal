---
id: TASK-111
title: MacroAction Class and Step Engine
status: closed
closed: 2026-04-18
opened: 2026-04-17
effort: Medium (2-8h)
complexity: Medium
human-in-loop: No
group: Macros
order: 2
prerequisites: [TASK-110]
---

## Description

Implement `MacroAction` — an `Action` subclass that owns a sequence of steps, where each step is a parallel group of actions. Steps execute in order; the next step begins only when all actions in the current step have `isInProgress() == false`.

## Acceptance Criteria

- [ ] `lib/PedalLogic/include/macro_action.h` created with the class interface below
- [ ] `lib/PedalLogic/src/macro_action.cpp` created with full implementation
- [ ] `addStep(Step)` appends a step (vector of `unique_ptr<Action>`) to the sequence
- [ ] `execute()` starts step 0 — fires all actions in `steps_[0]` simultaneously
- [ ] `update()` checks whether all actions in `currentStep_` have `isInProgress() == false`; if so advances and fires the next step
- [ ] `isInProgress()` returns `true` until the last step completes
- [ ] `executeRelease()` propagates to all actions in the currently active step
- [ ] `getType()` returns `Action::Type::Macro`
- [ ] For synchronous actions, `update()` may advance multiple steps in a single call (loop until blocked or done)
- [ ] Source file registered in `test/CMakeLists.txt` under `pedal_tests` so host tests compile it
- [ ] All existing tests still pass under `make test-host`

## Files to Touch

- `lib/PedalLogic/include/macro_action.h` (new)
- `lib/PedalLogic/src/macro_action.cpp` (new)
- `test/CMakeLists.txt` — add `macro_action.cpp` to the `pedal_tests` source list

## Test Plan

**Host tests** (`make test-host`): covered by TASK-114.

## Prerequisites

- **TASK-110** — `Action::Type::Macro` must exist before `getType()` can return it

## Notes

Class interface:

```cpp
class MacroAction : public Action {
public:
    using Step = std::vector<std::unique_ptr<Action>>;
    void addStep(Step step);

    void execute() override;
    void executeRelease() override;
    bool isInProgress() const override;
    void update();

    Action::Type getType() const override { return Action::Type::Macro; }
private:
    std::vector<Step> steps_;
    size_t currentStep_ = 0;
    bool running_ = false;
};
```

For actions that complete synchronously (most BLE and pin actions), `update()` should loop forward immediately rather than waiting for the next `loop()` tick. A `DelayedAction` step holds the macro in place until its timer expires.

Consider adding `virtual void update() {}` no-op to the `Action` base class to eliminate the `dynamic_cast` in `main.cpp` (see TASK-113).
