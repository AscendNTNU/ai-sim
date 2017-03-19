#include State.h

Drone State::getDrone(){
	return this.drone;
}
Robot State::getRobot(int index){
	return this.robots[target];
}
Robot State::getObstacle(int index){
	return this.obstacles[target];
}