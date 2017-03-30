#pragma once

#include "Robot.h"
#include "State.h"

class AI{
public:
    AI();
    State state;
    Robot chooseTarget(int num_Robots);
    action_t chooseAction();
    bool update(observation_t observation);
};
