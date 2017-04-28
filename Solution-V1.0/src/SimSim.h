#pragma once
#include "structs.h"
#define SIM_IMPLEMENTATION
#define SIM_CLIENT_CODE
#include "../../sim.h"
#include "../../gui.h"

class SimSim{
private:
	sim_State state;
	sim_Command cmd;
    sim_Observed_State observed_state;
    sim_Observed_State previous_state;

public:
	SimSim();
	bool getNewObservation();
	bool sendCommand(action_t action);
	observation_t updateObservation();
};


SimSim::SimSim(){
	sim_init_msgs(true);
}
bool SimSim::getNewObservation(){
	 sim_recv_state(&this->state);
     this->previous_state = this->observed_state;
     this->observed_state = sim_observe_state(state);
     return true;
}

sim_CommandType aiActionConverter(action_Type_t action){
	switch(action){

		case land_On_Top_Of:
			return sim_CommandType_LandOnTopOf;
		break;
		case land_In_Front_Of:
			return sim_CommandType_LandInFrontOf;
		break;
		case land_At_Point:
			return sim_CommandType_NoCommand;
		break;
		case search:
			return sim_CommandType_Search;
		break;
		default:
			return sim_CommandType_NoCommand;
		break;

	}
}

	// {
	//     no_Command = 0,   // continue doing whatever you are doing
	//     land_On_Top_Of,     // trigger one 45 deg turn of robot (i)
//     land_In_Front_Of,   // trigger one 180 deg turn of robot (i),
//     land_At_Point,	 // land at a given point
//     track,           // follow robot (i) at a constant height
//     search           // ascend to 3 meters and go to (x, y)
// };


    // sim_CommandType_NoCommand = 0,   // continue doing whatever you are doing
    // sim_CommandType_LandOnTopOf,     // trigger one 45 deg turn of robot (i)
    // sim_CommandType_LandInFrontOf,   // trigger one 180 deg turn of robot (i)
    // sim_CommandType_Track,           // follow robot (i) at a constant height
    // sim_CommandType_Search,          // ascend to 3 meters and go to (x, y)

bool SimSim::sendCommand(action_t action){
	std::cout << "there" << std::endl;
	this->cmd.type = aiActionConverter(action.type);
	this->cmd.x = action.where_To_Act.x;
	this->cmd.y = action.where_To_Act.y;

	std::cout << "target :" << action.target << std::endl;	
	this->cmd.i = action.target;
	std::cout << "target :" << cmd.i << std::endl;
	// for(int i = 0; i < 10; i++){
	// 	if(action.target.x == this->observed_state.target[i] && action.target.y == this->observed_state.target[i]){
	// 		cmd.i = i;
	// 	}
	// }
	sim_send_cmd(&this->cmd);
	return true;
}

//Update world
observation_t SimSim::updateObservation(){
	
	observation_t observation;

	//Simulation updater
	this->getNewObservation();

	observation.elapsed_time = this->observed_state.elapsed_time;

	observation.drone_x = this->observed_state.drone_x;
	observation.drone_y = this->observed_state.drone_y;

	observation.drone_cmd_done = this->observed_state.drone_cmd_done;

	for(int i = 0; i < 10; i++){
		observation.robot_x[i] = this->observed_state.target_x[i];
		observation.robot_y[i] = this->observed_state.target_y[i];
	}
	for(int i = 0; i < 4; i++){
		observation.obstacle_x[i] = this->observed_state.obstacle_x[i];
		observation.obstacle_y[i] = this->observed_state.obstacle_y[i];
	}

	return observation;
}