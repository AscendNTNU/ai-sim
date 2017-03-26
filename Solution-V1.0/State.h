#pragma once
#include "Robot.h"
#include "Drone.h"

class State{
private:
	Drone drone;
	Robot robots[10];
	Robot obstacles[4];
public:
	State();
	
	Drone getDrone();
	Robot getRobot(int index);
	Robot getObstacle(int index);

	bool updateState(observation_t observation);
};