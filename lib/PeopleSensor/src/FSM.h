#pragma once

#include <variant>
#include <string_view>
#include <Arduino.h>

namespace state {
    struct Start { };
    struct Idle { };
    struct Error { };
    struct FirstTriggeredEnter { };
    struct BothTriggeredEnter { };
    struct SecondTriggeredEnter { };
    struct FirstTriggeredLeave { };
    struct BothTriggeredLeave { };
    struct SecondTriggeredLeave { };
    struct SomeoneEntered { };
    struct SomeoneLeft { };
    using MachineState = std::variant<
            state::Start,
            state::Idle,
            state::Error,
            state::FirstTriggeredEnter,
            state::BothTriggeredEnter,
            state::SecondTriggeredEnter,
            state::FirstTriggeredLeave,
            state::BothTriggeredLeave,
            state::SecondTriggeredLeave,
            state::SomeoneEntered,
            state::SomeoneLeft
    >;
}

namespace event {
    struct FirstTriggered { };
    struct SecondTriggered { };
    struct BothTriggered { };
    struct NoneTriggered { };
    using PossibleEvent = std::variant<event::FirstTriggered, event::SecondTriggered, event::BothTriggered, event::NoneTriggered>;
}

namespace fsm {
    class FiniteStateMachine {
    public:
        state::MachineState currentState;

        FiniteStateMachine();

        void start();

        void processEvent(const event::PossibleEvent &event);

        std::string_view getCurrentStateName();

        bool hasPersonEntered();
        bool hasPersonLeft();

    private:
        bool hasEntered, hasLeft;
    };
}