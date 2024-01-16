#include <map>

#ifndef PROCESSSTATE_H
#define PROCESSSTATE_H

enum Process_State {
    CREATED,
    READY,
    RUNNING,
    BLOCKED,
    PREEMPT
};

enum Transition_State {
    UNKNOWN,
    TRANS_TO_READY,
    TRANS_TO_PREEMPT,
    TRANS_TO_RUN,
    TRANS_TO_BLOCK
};

static Transition_State ProcessStateToTransitionState(Process_State state) {

            std::map<Process_State, Transition_State> typeMap;

            typeMap[Process_State::READY] = Transition_State::TRANS_TO_READY;
            typeMap[Process_State::RUNNING] = Transition_State::TRANS_TO_RUN;
            typeMap[Process_State::BLOCKED] = Transition_State::TRANS_TO_BLOCK;
            typeMap[Process_State::PREEMPT] = Transition_State::TRANS_TO_PREEMPT;
        
            std::map<Process_State, Transition_State>::iterator it = typeMap.find(state);
            if (it != typeMap.end()) {
                return it->second;
            } else {
                return Transition_State::UNKNOWN; // Handle unknown values
            }
        }


#endif