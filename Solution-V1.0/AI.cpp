#include "AI.h"
#include "Plank.h"


AI::AI(){
    this->state = new State();
}

Robot AI::chooseTarget(int num_Robots){
    float max_reward = -200000;
    float reward = 0;
	bool robotChosen = false;
    Robot* target = NULL;

    float timeToTurn = 20 - fmod(state.time,20);

    for(int i = 0; i < this->state.getNumRobots(); i++){
        Robot* robot = *this->state.robots[i];
        if (!robot->isMoving()) {
            // Robot is turning. Do we have correct angle?
        }

		if(robot->plank->willExitGreen()) {
            // Ignore it
			continue;
		}

        reward = robot->plank->getReward();

        if(reward > max_reward){
            max_reward = reward;
            target = robot;
			robotChosen = true;
        }
    }
	if(!robotChosen) {
		// Found no target! What to do?
	}
    return target
}
action_t AI::chooseAction(Robot target){
    point_t interception = this->state.drone->stategetInterceptPoint(target);
    point_t step_Point = {.x = interseption.x, .y = interseption.y};
    float time_after_interseption = 0;

    float n = 10;
    float step_size = target.plank->getLength()/n;
    float angle = target.plank->getAngle();
    float step_x = step_size*cos(angle);
    float step_y = step_size*sin(angle);

    // Temporary max rewarded action
    action_t best_Action = action_empty
    best_Action.where_To_Act.travel_Time = interseption.travel_Time;
    
    action_t step_Action;
    bool backwards = false;
    int i = 1;
    while (i > 0) {
        if (target.plank->isOutsideOfPlank(step_Point)) {
            // End of plank was reached
            if (backwards) {
                return best_Action;
            } else {
                i = n+1;
                backwards = true;
                angle += MATH_PI;
            }
        }
        step_Action = getBestActionAtPosition(target, step_Point, time_after_interseption);

        if (step_Action.reward > best_Action.reward) {
            best_Action = step_Action;
            best_Action.when_To_Act = time_after_interseption + interseption.travel_Time;
        }

        if (backwards) {
            step_Point = {.x = x-step_x, .y = y-step_y};
            i -= 1;
        } else {
            step_Point = {.x = x+step_x, .y = y+step_y};
            i += 1;
        }
        time_after_interseption = time_after_interseption + (step_size)/target->getSpeed();

    }
    return best_Action;
}

action_t getBestActionAtPosition(Robot target, point_t position, float time_after_interseption) {
    int num_Iterations = 5; // Number of iterations when summing along a plank
    action_t action;
    action.where_To_Act = position;
    float time_Until_Turn = fmod(state.getTimeStamp() + position.travel_time + time_after_interseption, 20);

    Plank plank_On_Top = new Plank(position, fmod(target->getAngle() + (MATH_PI/4), 2*MATH_PI), time_Until_Turn, num_Iterations);
    Plank plank_In_Front = new Plank(position, fmod(target->getAngle() + MATH_PI, 2*MATH_PI) time_Until_Turn, num_Iterations);

    return actionWithMaxReward(plank_On_Top.getReward(), plank_In_Front.getReward(), action);
}

action_t actionWithMaxReward(float reward_On_Top, float reward_In_Front, action_t action){
    if(reward_On_Top > reward_In_Front){
        action.type = ai_landingOnTop;
        action.reward = reward_On_Top;
    } else if (reward_On_Top < reward_In_Front){
        action.type = ai_landingInFront;
        action.reward = reward_In_Front;
    } else if (reward_On_Top == reward_In_Front){
        // Return in front because it is easier?
        action.type = ai_landingInFront;
        action.reward = reward_In_Front;
    } else {
        // Will it ever get here?
        action.type = ai_waiting;
        action.reward = 0;
    }
    return action;
}

bool AI::update(observation_t observation){
    this->state.updateState(observation);
}