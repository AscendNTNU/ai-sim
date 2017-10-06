#include "AI.h"

AI::AI(){
    this->state = new State();
}

Robot* AI::chooseTarget(int num_Robots){
    float max_reward = -200000;
    float reward = 0;
	bool robotChosen = false;
    Robot* target = NULL;

    // get this from 20 - target->getTimeAfterTurn() if needed
    // float timeToTurn = 20 - fmod(this->state->getTimeStamp(),20);

    for(int i = 0; i < num_Robots; i++){
        Robot* robot = this->state->robots[i];
        if (robot)
        if (!robot->isMoving()) {
            std::cout << "Robot is turning. Do we have correct angle?" << std::endl;
            continue;
        }
		if(robot->current_Plank->willExitGreen()) {
            std::cout <<  "Robot will exit green line. Ignoring it" << std::endl;
			continue;
		}

        if(robot->outOfField()){
            std::cout << "out of field" << std::endl;
            continue;
        }

        reward = robot->current_Plank->getReward();
        if(reward > max_reward){
            max_reward = reward;
            target = robot;
			robotChosen = true;
        }
    }
	if(!robotChosen) {
        std::cout << "No target chosen! Error" << std::endl;
        Robot* dummy = new Robot(-1);
        target = dummy;
		std::cout << "Found no target! What to do?" << std::endl;
	}
    return target;
}
action_t AI::chooseAction(Robot* target){
    point_t interception = this->state->drone->getInterceptPoint(target);
    point_t step_Point = {
        .x = interception.x, 
        .y = interception.y
    };
    std::cout << "Intercept: " << step_Point << std::endl;
    std::cout << "Target: " << std::endl;
    std::cout << *target << std::endl;

    float time_after_interception = 0;

    float n = 10;
    float step_size = target->current_Plank->getLength()/n;
    float angle = target->current_Plank->getAngle();
    float step_x = step_size*cos(angle);
    float step_y = step_size*sin(angle);

    // Temporary max rewarded action
    action_t best_Action = action_Empty;
    
    float start_time = this->state->getTimeStamp();
    float time_stamp = start_time + interception.travel_Time;
    action_t step_Action;
    bool backwards = false;
    int i = 1;
    while (i > 0) {
        if (target->current_Plank->pointIsOutsideOfPlank(step_Point)) {
            if (backwards) {
                best_Action.target = target->getIndex();
                std::cout << "Best action: "<< std::endl << best_Action.when_To_Act << std::endl;
                std::cout << "Time expected: " << best_Action.when_To_Act << std::endl;

                return best_Action;
            } else {
                i = n+1;
                backwards = true;
                angle += MATH_PI;
            }
        }
		else {
			step_Action = getBestActionAtPosition(target, step_Point, time_stamp);
			if (step_Action.reward > best_Action.reward) {
				best_Action = step_Action;
				best_Action.when_To_Act = time_stamp;//time_after_interception + interception.travel_Time;// Denne skal kanskje være globalt tispunkt etter start?
			
            }
		}
        
		
		if (backwards) {
            step_Point.x = step_Point.x-step_x;
            step_Point.y = step_Point.y-step_y;
            i -= 1;
        } else {
            step_Point.x = step_Point.x+step_x; 
            step_Point.y = step_Point.y+step_y;
            i += 1;
        }
        time_stamp += (step_size)/target->getSpeed();

    }

    std::cout << "Best action: "<< std::endl << best_Action.when_To_Act << std::endl;
    std::cout << "Time expected: " << best_Action.when_To_Act << std::endl;

    best_Action.target = target->getIndex();
    return best_Action;
}

action_t AI::getBestActionAtPosition(Robot* target, point_t position, float timeStamp) {
    int num_Iterations = 5; // Number of iterations when summing along a plank
    action_t action;
    action.where_To_Act = position;
    float time_After_Turn_Start = fmod(timeStamp, 20);

    Plank* plank_On_Top = new Plank(position, fmod(target->getOrientation() + (MATH_PI/4), 2*MATH_PI), 
                                    time_After_Turn_Start, num_Iterations);
    Plank* plank_In_Front = new Plank(position, fmod(target->getOrientation() + MATH_PI, 2*MATH_PI), 
                                    time_After_Turn_Start, num_Iterations);
    if(time_After_Turn_Start < 2 or time_After_Turn_Start > 19){
        action.reward = -200000;
        return action;
    }

    return actionWithMaxReward(plank_On_Top->getReward(), plank_In_Front->getReward(), action);
}

action_t AI::actionWithMaxReward(float reward_On_Top, float reward_In_Front, action_t action){
    if(reward_On_Top > reward_In_Front){
        action.type = land_On_Top_Of;
        action.reward = reward_On_Top;
    } else if (reward_On_Top < reward_In_Front){
        action.type = land_In_Front_Of;
        action.reward = reward_In_Front;
    } else if (reward_On_Top == reward_In_Front){
        // Return in front because it is easier?
        action.type = land_In_Front_Of;
        action.reward = reward_In_Front;
    } else {
        // Will it ever get here?
        action.type = no_Command;
        action.reward = 0;
    }
    action.type = land_In_Front_Of;
    return action;
}

bool AI::update(observation_t observation) {
    this->state->updateState(observation);
    return true;
}