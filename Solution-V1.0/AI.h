#pragma once

#include "Robot.h"
#include "State.h"

class AI{
private:
    State state;
    State previous_State;
    Robot target;
public:
    AI();
    Robot chooseTarget(int num_Robots);
    Action chooseAction();
    void executeAction();
    bool update(observation_t observation);
};
