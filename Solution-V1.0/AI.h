#pragma once

#include "Robot.h"
#include "State.h"

class AI{
private:
    State state;
public:
    AI();
    Robot chooseTarget(State current_State, State previous_State, int num_Robots);
    Action chooseAction(State observed_State, Robot target);
    void executeAction(State observed_State, Action action);
    bool update(observation_t observation);
};
