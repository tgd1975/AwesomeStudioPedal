#pragma once
#include "action.h"
#include <cstddef>
#include <memory>
#include <vector>

class MacroAction : public Action
{
public:
    using Step = std::vector<std::unique_ptr<Action>>;

    void addStep(Step step);

    void execute() override;
    void executeRelease() override;
    bool isInProgress() const override { return running_; }
    void update();

    Action::Type getType() const override { return Action::Type::Macro; }

    void getJsonProperties(JsonObject& json) const override;

    /// Read-only access to the configured steps; used by the serialiser and
    /// host tests to verify round-trip equivalence.
    const std::vector<Step>& getSteps() const { return steps_; }

private:
    void fireCurrentStep();
    bool currentStepDone() const;

    std::vector<Step> steps_;
    size_t currentStep_ = 0;
    bool running_ = false;
};
