#include "structs.h"
#define SIM_IMPLEMENTATION
#define SIM_CLIENT_CODE
#include "../sim.h"
#include "../gui.h"

class Input{
private:
	sim_State state;
    sim_Observed_State observed_state;
    sim_Observed_State previous_state;

public:
	bool initializeSim();
	bool getNewObservation();
};