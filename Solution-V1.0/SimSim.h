#pragma once

#include "structs.h"
#define SIM_IMPLEMENTATION
#define SIM_CLIENT_CODE
#include "../sim.h"
#include "../gui.h"

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