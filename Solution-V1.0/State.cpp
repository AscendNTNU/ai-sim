#include "State.h"

State::State(){
	this.drone = new Drone();
	
	for(int i = 0, i<10; i++){
		this.robot[i] = new Robot();
	}
	
	for(int i = 0, i<4; i++){
		this.obstacles[i] = new Robot();
	}
}

Drone State::getDrone(){
	return this.drone;
}
Robot State::getRobot(int index){
	return this.robots[index];
}
Robot State::getObstacle(int index){
	return this.obstacles[index];
}
