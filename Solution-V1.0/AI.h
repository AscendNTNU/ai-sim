#pragma once

#include "Robot.h"
#include "State.h"

class AI{
public:
    AI();
    State* state;
    Robot* chooseTarget(int num_Robots);
    action_t chooseAction(Robot* target);
    bool update(observation_t observation);
    action_t getBestActionAtPosition(Robot* target, point_t position, float time_after_interception);
    action_t actionWithMaxReward(float reward_On_Top, float reward_In_Front, action_t action);
};