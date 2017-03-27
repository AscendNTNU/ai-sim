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

//Update world
bool SimIn::update_world(){
	
	observation_t observation;

	//Simulation updater
	sim.getNewObservation();
	this.time = input.time;

	observation.elapsed_time = sim.elapsed_time;

	observation.drone_x = sim.drone_x;
	observation.drone_y = sim.drone_y;

	observation.drone_cmd_done = sim.drone_cmd_done;

	for(int i = 0; i < 10; i++){
		observation.target_x[i] = sim.target_x[i];
		observation.target_y[i] = sim.target_y[i];
	}
	for(int i = 0; i < 4; i++){
		observation.obstacle_x[i] = sim.obstacle_x[i];
		observation.obstacle_y[i] = sim.obstacle_y[i];
	}

	return observation;
}