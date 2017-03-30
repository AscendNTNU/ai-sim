#include "SimIn.h"

bool SimIn::initializeSim(){
	sim_init_msgs(true);
	return true;
}
bool SimIn::getNewObservation(){
	 sim_recv_state(&this->state);
     this->previous_state = this->observed_state;
     this->observed_state = sim_observe_state(state);
     return true;
}

bool SimIn::sendCommand(action_t action){
	//Todo : action stuct to sim action
	// cmd.type = sim_CommandType_LandInFrontOf;
	// cmd.x = action.where_To_Act.x;
	// cmd.y = action.where_To_Act.y;

	// for(int i = 0; i < 10; i++){
	// 	if(action.target.x == this->observed_state.target[i] && action.target.y == this->observed_state.target[i]){
	// 		cmd.i = i;
	// 	}
	// }
	// sim_send_cmd(&cmd);
	return true;
}

//Update world
observation_t SimIn::update_world(){
	
	observation_t observation;

	//Simulation updater
	this->getNewObservation();

	observation.elapsed_time = this->observed_state.elapsed_time;

	observation.drone_x = this->observed_state.drone_x;
	observation.drone_y = this->observed_state.drone_y;

	observation.drone_cmd_done = this->observed_state.drone_cmd_done;

	for(int i = 0; i < 10; i++){
		observation.target_x[i] = this->observed_state.target_x[i];
		observation.target_y[i] = this->observed_state.target_y[i];
	}
	for(int i = 0; i < 4; i++){
		observation.obstacle_x[i] = this->observed_state.obstacle_x[i];
		observation.obstacle_y[i] = this->observed_state.obstacle_y[i];
	}

	return observation;
}