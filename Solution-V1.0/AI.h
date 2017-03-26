#pragma once

#include "World.h"

class AI{
private:
    State state;
public:
    AI();
    Robot AI::chooseTarget(State current_State, State previous_State, int num_Robots);
    Action AI::chooseAction(State observed_State, Robot target);
    void AI::executeAction(State observed_State, Action action);
    bool update(observation_t observation);
}
