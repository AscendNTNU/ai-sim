#include "Robot.h"
#include "Drone.h"

class State{
private:
	Drone drone;
	Robots robots[10];
	Robots obstacles[4];
public:
	Drone getDrone();
	Robot getRobot(int index);
	Robot getObstacle(int index);
};