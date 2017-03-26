#pragma once
#include "SimIn.h"

bool SimIn::initializeSim(){
	sim_init_msgs(true);
	return true;
}
bool SimIn::getNewObservation(){
	 sim_recv_state(&this.state);
     this.previous_state = this.observed_state;
     observed_state = sim_observe_state(state);
     return true;
}

bool SimIn::sendCommand(action_t action){
	cmd.type = action.type;
	cmd.x = action.x;
	cmd.y = action.y;

	for(int i = 0; i < 10; i++){
		if(action.target.x == this.observed_state.target[i] && action.target.y == this.observed_state.target[i]){
			cmd.i = i;
		}
	}
	sim_send_cmd(&cmd);
	return true;
}