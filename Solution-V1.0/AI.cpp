#pragma once

#include "AI.h"


AI::AI(){
    this.state* = new State();
    this.previous_State = NULL;
    this.target = NULL;
}
Robot AI::chooseTarget(int num_Robots){
    float max_reward = -200000;
    float reward = 0;
	bool robotChosen = false;
    Robot* target = NULL;

    float timeToTurn = 20 - fmod(state.time,20);

    for(int i = 0; i < this.state.getNumRobots(); i++){
        Robot* robot = *state.robots[i];
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
    this.target = target;
    return target
}
Action AI::chooseAction(){
    point_t interception = this.state.drone->stategetInterceptPoint(target);
    float temp = target.intersection.travel_time;
    //target.intersection.travel_time = 0;
    point_t step_point;
    step_point.x = intersection.x;
    step_point.y = intersection.y;
    float time_after_interseption = 0;

    float n = 10;
    float step_size = target->plank.getLength()/n;
    float angle = target->plank->getAngle()
    float step_x = step_size*cos(angle);
    float step_y = step_size*sin(angle);

    // Temporary max rewarded action
    action_t best_Action;
    best_Action.reward = -1000;
    best_Action.action_Type_t = ai_waiting;
    best_Action.where_To_Act = point_Zero;
    best_Action.where_To_Act.travel_Time = interseption.travel_Time;
    
    action_t step_Action;
    bool backwards = false;
    int i = 1;
    while (i > 0) {
        if (target->plank->isOutsideOfPlank(step_point)) {
            // End of plank was reached
            if (backwards) {
                return best_Action;
            } else {
                i = n+1;
                backwards = true;
                angle += MATH_PI;
            }
        }
        step_Action = getBestActionAtPoint(target, step_point, time_after_interseption);

        if (step_Action.reward > best_Action.reward) {
            best_Action = step_Action;
            best_Action.x = x;
            best_Action.y = y;
            best_Action.time_after_interseption = time_after_interseption;
            best_Action.time_until_intersection = target.intersection.travel_time;
        }

        

        if (backwards) {
            x = x-step_x;
            y = y-step_y;
            i -= 1;
        } else {
            x = x+step_x;
            y = y+step_y;
            i += 1;
        }
        time_after_interseption = time_after_interseption + (step_size)/Robot_Speed;

    }
    best_Action.time_until_intersection = temp;
    return best_Action;
}

action_t AI::getBestAction(point_t point, float time_after_interseption) {
    action_t action;

    Plank plankOnTop = createPlank(x, y, wrap_angle(target.angle + 0.785),//0.785 radians is almost 45 degerees 
            (int)(state.elapsed_time+target.intersection.travel_time+time_after_interseption) % 20);
    Plank plankInFront = createPlank(x, y, wrap_angle(target.angle + 3.14), 
            (int)(state.elapsed_time+target.intersection.travel_time+time_after_interseption) % 20);

    float max_reward = (std::max)(plankOnTop.getReward(),plankInFront.getReward());
    if(max_reward == plankInFront.getReward()){
        action.action_Type_t = ai_landingInFront;
        action.reward = plankInFront.getReward();
    }
    else if(max_reward == plankOnTop.getReward()){
        action.action_Type_t = ai_landingOnTop;
        action.reward = plankOnTop.getReward();
    }
    else{
        action.action_Type_t = ai_waiting;
    }
    return action;
}

void AI::executeAction(State state, Action action){

}

bool AI::update(observation_t observation){
    this.previous_State = state;
    this.state.update(observation);
}