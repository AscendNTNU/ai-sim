#include "World.h"

World::World(){
	this.origin = point_Zero;
	this.time = 0;
	float orientation = 0;
	bounds = {x_Max = 20, y_Max = 20; 
	AI ai = new AI();
	Input input = new Input();
}

point_t World::getOrigin(){
	return this.origin;
}
float World::getCurrentTime(){
	return this.time;
}
float World::getOrientation(){
	return this.orientation;
}
bounds_t World::getBounds(){
	return this.bounds_t;
}



bool World::startTimer(){

	input.initializeSim();
	return true;
}

bool World::update_world(){
	
	//Simulation updater
	input.getNewObservation();
	this.time = input.time;

	//Update drone
	point_t drone_Position = {input.drone_x, input.drone_y, 1}
	ai.drone.update(drone_position, drone.cmd_done);


	//Update robots
	for(int i = 0; i < 10; i++){
		point_t robot_Position = {input.target_x[i], input.target_y[i], 0}
		ai.robot[i].update(robot_Position, input.target_q[i])
	}

	//Update obstacles
	for(int i = 0; i < 4; i++){
		point_t robot_Position = {input.obstacle_x[i], obstacle.target_y[i], 0}
		ai.robot[i].update(robot_Position, input.obstacle_q[i])
	}

}