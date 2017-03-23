#include "Sim.h"

bool Sim::initializeSim(){
	sim_init_msgs(true);
	return true;
}
bool Sim::getNewObservation(){
	 sim_recv_state(&this.state);
     this.previous_state = this.observed_state;
     observed_state = sim_observe_state(state);
     return true;
}

bool Sim::sendCommand(){
	return false; //Todo
}