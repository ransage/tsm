#pragma once

#include "Event.h"
#include "State.h"
#include "StateMachineDef.h"

namespace tsm {
template<typename HSMDef>
struct HSMBehavior : public HSMDef
{
    using Transition = typename StateMachineDef<HSMDef>::Transition;

    HSMBehavior(State* parent = nullptr)
      : HSMDef(parent)
    {}

    virtual ~HSMBehavior() = default;

    void startSM() { this->onEntry(Event::dummy_event); }

    void stopSM() { this->onExit(Event::dummy_event); }

    // traverse the hsm hierarchy down.
    State* dispatch(State* state) const
    {
        State* parent = state;
        State* kid = parent->getCurrentState().get();
        while (kid->getParent()) {
            parent = kid;
            kid = kid->getCurrentState().get();
        }
        return parent;
    }

    void execute(Event const& nextEvent) override
    {
        LOG(INFO) << "Current State:" << this->currentState_->name
                  << " Event:" << nextEvent.id;

        HSMDef* thisDef = static_cast<HSMDef*>(this);
        shared_ptr<Transition> t =
          thisDef->next(this->currentState_, nextEvent);

        if (!t) {
            // If transition does not exist, pass event to parent HSM
            if (this->parent_) {
                // TODO(sriram) : should call onExit? UML spec seems to say yes
                // invoking onExit() here will not work for Orthogonal state
                // machines
                // this->onExit(nextEvent);
                this->parent_->execute(nextEvent);
            } else {
                LOG(ERROR) << "Reached top level HSM. Cannot handle event";
            }
        } else {
            // Evaluate guard if it exists
            bool result = t->guard && (thisDef->*(t->guard))();

            if (!(t->guard) || result) {
                // Perform entry and exit actions in the doTransition function.
                // If just an internal transition, Entry and exit actions are
                // not performed
                t->template doTransition<HSMDef>(thisDef);

                this->currentState_ = t->toState;

                DLOG(INFO) << "Next State:" << this->currentState_->name;
            } else {
                LOG(INFO) << "Guard prevented transition";
            }
            if (this->currentState_ == this->getStopState()) {
                DLOG(INFO) << this->name << " Done Exiting... ";
                this->onExit(Event::dummy_event);
            }
        }
    }

    shared_ptr<State> const getCurrentState() const override
    {
        DLOG(INFO) << "GetState : " << this->name;
        return this->currentState_;
    }
};

} // namespace tsm