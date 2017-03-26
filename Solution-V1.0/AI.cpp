#pragma once

#include "AI.h"


AI::AI(){
    this.state* = new State();
}
Robot AI::chooseTarget(State current_State, State previous_State, int num_Robots){
    float max_value = -200000;
    float temp_value = 0;
    int index = 0;
	bool robotChosen = false;

    float timeToTurn = 20 - fmod(current_State.time,20);

    for(int i = 0; i < this.current_State.getNumRobots(); i++){
        Robot* robot = *observed_State.robots[i];
        if (!robot->isMoving()) {
            // Robot is turning. Do we have correct angle
            // Do we have correct angle?
        }

		if(robot.plank.willExitGreen()) {
            // Ignore it
			continue;
		}

        temp_value = getPlankValue(gridValue, plank, angle, 5);

        if(temp_value > max_value){
            max_value = temp_value;
            target.index = i;
            target.plank = plank;
            target.angle = wrap_angle(observed_state.target_q[i]);
            target.currentReward = temp_value;
			robotChosen = true;
        }
    }
	if(!robotChosen) {
		std::cout << "NO ROBOT CHOSEN! Setting index to -1." << std::endl;
		target.index = -1;
		target.plank = createPlank(observed_state.target_x[1], observed_state.target_y[1],
									wrap_angle(observed_state.target_q[1]), timeToTurn);;
		target.angle = wrap_angle(observed_state.target_q[1]);
		target.currentReward = getPlankValue(gridValue, target.plank, target.angle, 5);
	}
	std::cout << "Chose Target: " << target.index << std::endl;
	std::cout << "Target Value: " << max_value    << std::endl;
	std::cout << "Target (X, Y): " << observed_state.target_x[target.index] << ", " << observed_state.target_y[target.index] << std::endl;

    return target;
}
Action AI::chooseAction(State observed_State, Robot target){

}
void AI::executeAction(State observed_State, Action action){

}

bool AI::update(observation_t observation){
    this.state.update(observation);
}