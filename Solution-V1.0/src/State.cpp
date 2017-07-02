#include "State.h"

State::State(){
	this->drone = new Drone();
	
	for(int i = 0; i<10; i++){
		this->robots[i] = new Robot();
	}
	
	for(int i = 0; i<4; i++){
		this->obstacles[i] = new Robot();
	}
	this->time_Stamp = 0;
}

Drone* State::getDrone(){
	return this->drone;
}
Robot* State::getRobot(int index){
	return this->robots[index];
}
Robot* State::getObstacle(int index){
	return this->obstacles[index];
}

float State::getTimeStamp(){
	return this->time_Stamp;
}

bool State::updateState(observation_t observation){
	point_t position = point_Zero;
	this->drone->update(observation);
	this->time_Stamp = observation.elapsed_time;
	for(int i = 0; i < 10; i++){
		position = (point_t){.x = observation.robot_x[i], .y = observation.robot_y[i]};
		this->robots[i]->update(i, position, observation.robot_q[i], observation.elapsed_time);
	}
	for(int i = 0; i < 4; i++){
		position = (point_t){.x = observation.obstacle_x[i], .y = observation.obstacle_y[i]};
		this->obstacles[i]->update(i, position, observation.obstacle_q[i], observation.elapsed_time);
	}
	return true;
}