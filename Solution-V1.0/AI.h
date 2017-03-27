#pragma once

#include "Robot.h"
#include "State.h"

class AI{
public:
    AI();
    State state;
    Robot chooseTarget(int num_Robots);
    Action chooseAction();
    bool update(observation_t observation);
};
