#pragma once
#include "State.h"

State::State(){
	this.drone = new Drone();
	
	for(int i = 0, i<10; i++){
		this.robot[i]* = new Robot();
	}
	
	for(int i = 0, i<4; i++){
		this.obstacles[i]* = new Robot();
	}
	this.time_Stamp = world.getCurrentTime();
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


bool State::updateState(observation_t observation){
	this.drone.update(observation);
	for(int i = 0; i < 10; i++){
		this.robots[i].update(observation.robot_x[i], observation.robot_y[i], observation.robot_q[i]);
	}
	for(int i = 0; i < 4; i++){
		this.obstacles[i].update(observation.obstacle_x[i], observation.obstacle_y[i], observation.obstacle_q[i]);
	}
}