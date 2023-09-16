#include "FSM.h"

/* All transition functions out FSM needs to work correctly */
namespace transition {
    using state::Start, state::Idle, state::Error, state::FirstTriggeredEnter, state::BothTriggeredEnter,
    state::SecondTriggeredEnter, state::FirstTriggeredLeave, state::BothTriggeredLeave, state::SecondTriggeredLeave, 
    state::SomeoneEntered, state::SomeoneLeft, state::MachineState;
    using event::FirstTriggered, event::SecondTriggered, event::BothTriggered, event::NoneTriggered;

    /* Start state */
    MachineState onEvent(const Start &, const auto &) {
        return Idle{};
    }

    /* Idle state */
    MachineState onEvent(const Idle &, const FirstTriggered &) {
        return FirstTriggeredEnter{};
    }

    MachineState onEvent(const Idle &, const SecondTriggered &) {
        return SecondTriggeredLeave{};
    }

    MachineState onEvent(const Idle &, const NoneTriggered &) {
        return Idle{};
    }

    /* SomeoneEntered / -Left states */
    MachineState onEvent(const SomeoneEntered, const auto &) {
        return Idle{};
    }

    MachineState onEvent(const SomeoneLeft, const auto &) {
        return Idle{};
    }

    /* Entering states */
    /* FirstTriggeredEnter state */
    MachineState onEvent(const FirstTriggeredEnter &, const FirstTriggered &) {
        return FirstTriggeredEnter{};
    }

    MachineState onEvent(const FirstTriggeredEnter &, const BothTriggered &) {
        return BothTriggeredEnter{};
    }

    MachineState onEvent(const FirstTriggeredEnter &, const NoneTriggered &) {
        return Idle{};
    }

    /* BothTriggeredEnter state */
    MachineState onEvent(const BothTriggeredEnter &, const FirstTriggered &) {
        return FirstTriggeredEnter{};
    }

    MachineState onEvent(const BothTriggeredEnter &, const SecondTriggered &) {
        return SecondTriggeredEnter{};
    }

    MachineState onEvent(const BothTriggeredEnter &, const BothTriggered &) {
        return BothTriggeredEnter{};
    }

    /* SecondTriggeredEnter state */
    MachineState onEvent(const SecondTriggeredEnter &, const SecondTriggered &) {
        return SecondTriggeredEnter{};
    }

    MachineState onEvent(const SecondTriggeredEnter &, const BothTriggered &) {
        return BothTriggeredEnter{};
    }

    MachineState onEvent(const SecondTriggeredEnter &, const NoneTriggered &) {
        return SomeoneEntered{};
    }

    /* Leaving states */
    /* SecondTriggeredLeave state */
    MachineState onEvent(const SecondTriggeredLeave &, const SecondTriggered &) {
        return SecondTriggeredLeave{};
    }

    MachineState onEvent(const SecondTriggeredLeave &, const BothTriggered &) {
        return BothTriggeredLeave{};
    }

    MachineState onEvent(const SecondTriggeredLeave &, const NoneTriggered &) {
        return Idle{};
    }

    /* BothTriggeredLeave state */
    MachineState onEvent(const BothTriggeredLeave &, const FirstTriggered &) {
        return FirstTriggeredLeave{};
    }

    MachineState onEvent(const BothTriggeredLeave &, const SecondTriggered &) {
        return SecondTriggeredLeave{};
    }

    MachineState onEvent(const BothTriggeredLeave &, const BothTriggered &) {
        return BothTriggeredLeave{};
    }

    /* FirstTriggeredLeave state */
    MachineState onEvent(const FirstTriggeredLeave &, const FirstTriggered &) {
        return FirstTriggeredLeave{};
    }

    MachineState onEvent(const FirstTriggeredLeave &, const BothTriggered &) {
        return BothTriggeredLeave{};
    }

    MachineState onEvent(const FirstTriggeredLeave &, const NoneTriggered &) {
        return SomeoneLeft{};
    }

    /* Default error transition function. Every function which is not defined returns the Error state */
    MachineState onEvent(const auto &, const auto &) {
        return Error{};
    }
}

namespace fsm {
    /* Helper for std::visit */
    template<class... Ts>
    struct overloaded : Ts ... {
        using Ts::operator()...;
    };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    FiniteStateMachine::FiniteStateMachine() : hasEntered{false}, hasLeft{false} {}

    /* (Re-)set the machine in its initial state */
    void FiniteStateMachine::start() {
        currentState = state::Start{};
    }

    /* Handle events like entry sensor got triggered */
    void FiniteStateMachine::processEvent(const event::PossibleEvent &event) {
        currentState = std::visit(overloaded{
                [](const auto &state, const auto &evt) {
                    return transition::onEvent(state, evt);
                }}, currentState, event);
    }

    /* Convert the current state into a readable string. Mostly for debugging reasons */
    std::string_view FiniteStateMachine::getCurrentStateName() {
        return std::visit(overloaded{
                                  [](const state::Start &) { return "Start"; },
                                  [](const state::Idle &) { return "Idle"; },
                                  [](const state::Error &) { return "Error"; },
                                  [](const state::FirstTriggeredEnter &) { return "FirstTriggeredEnter"; },
                                  [](const state::BothTriggeredEnter &) { return "BothTriggeredEnter"; },
                                  [](const state::SecondTriggeredEnter &) { return "SecondTriggeredEnter"; },
                                  [](const state::FirstTriggeredLeave &) { return "FirstTriggeredLeave"; },
                                  [](const state::BothTriggeredLeave &) { return "BothTriggeredLeave"; },
                                  [](const state::SecondTriggeredLeave &) { return "SecondTriggeredLeave"; },
                                  [](const state::SomeoneEntered &) { return "SomeoneEntered"; },
                                  [](const state::SomeoneLeft &) { return "SomeoneLeft"; },
                                  [](const auto &) { return "\n\n\nno-state???\n\n\n"; }
                          }, currentState
        );
    }

    /* This function can be used to poll whether a person entered. hasEntered is immediately reset afterwards */
    bool FiniteStateMachine::hasPersonEntered() {
        if (hasEntered) {
            hasEntered = false;
            return true;
        }
        return false;
    }

    /* Same as above but with hasLeft */
    bool FiniteStateMachine::hasPersonLeft() {
        if (hasLeft) {
            hasLeft = false;
            return true;
        }
        return false;
    }

}

