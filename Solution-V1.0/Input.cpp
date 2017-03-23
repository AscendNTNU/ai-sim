#include "Input.h"

bool Input::initializeSim(){
	sim_init_msgs(true);
	return true;
}
bool Input::getNewObservation(){
	 sim_recv_state(&this.state);
     this.previous_state = this.observed_state;
     observed_state = sim_observe_state(state);
     return true;
}