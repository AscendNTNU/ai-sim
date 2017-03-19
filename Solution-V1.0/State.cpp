#include State.h

Drone State::getDrone(){
	return this.drone;
}
Robot State::getRobot(int target){
	return this.robots[target];
}
Robot State::getObstacle(int target){
	return this.obstacles[target];
}