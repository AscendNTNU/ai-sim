#include "World.h"

AI::AI(){

}
Robot AI::chooseTarget(State observed_State, State previous_State){
    float max_value = -200000;
    float temp_value = 0;
    int index = 0;
    Target target;
	bool robotChosen = false;

    float timeToTurn = 20 - fmod(observed_state.elapsed_time,20);

    for(int i = 0; i < Num_Targets; i++){
		std::cout << "Checking target: " << i << " at (X, Y): (" << observed_state.target_x[i] << ", " << observed_state.target_y[i] << ")" << std::endl;
		if(!observed_state.target_removed[i]){
			std::cout << "Target not removed" << std::endl;
            if (!targetIsMoving(i, previous_State, observed_state))
            {
                target.index = -1;
                return target;
                //std::cout << "Target not moving" << std::endl;
            }

            float angle = wrap_angle(observed_state.target_q[i]);

            Plank plank = createPlank(observed_state.target_x[i], observed_state.target_y[i],
                angle, timeToTurn);

			if(plank.goingOutGreen) {
				std::cout << "Target already going out of green line!" << std::endl;
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