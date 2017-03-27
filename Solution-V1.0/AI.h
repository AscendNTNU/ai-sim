#pragma once

#include "Robot.h"
#include "State.h"

class AI{

public:
    AI();
    Robot chooseTarget(int num_Robots);
    void executeAction();
    Action chooseAction();
    bool update(observation_t observation);
};
