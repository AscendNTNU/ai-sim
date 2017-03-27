#pragma once
#include "Robot.h"
#include "Drone.h"

class State{
private:
	float time_Stamp;
public:
	Robot robots[10];
	Robot obstacles[4];
<<<<<<< HEAD
	State();
	Drone* drone;
=======
	Drone drone;
>>>>>>> b1e3c6bdd496622a74fae285e3179f4d6868a806
	
	State();

	Drone getDrone();
	Robot getRobot(int index);
	Robot getObstacle(int index);
	float getTimeStamp();

	bool updateState(observation_t observation);
};