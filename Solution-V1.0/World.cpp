#pragma once
#include "World.h"


//Constructors
World::World(){
	this.origin = point_Zero;
	this.time = 0;
	float orientation = 0;
	bounds = {x_Max = 20, y_Max = 20}; 
	AI ai = new AI();
	Input input = new Input();
}


//Get
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


//Initialize
bool World::startTimer(){

	input.initializeSim();
	return true;
}

//Update world
bool World::update_world(){
	
	observation_t = observation;

	//Simulation updater
	sim.getNewObservation();
	this.time = input.time;

	observation.elapsed_time = sim.elapsed_time;

	observation.drone_x = sim.drone_x;
	observation.drone_y = sim.drone_y;

	observation.drone_cmd_done = sim.drone_cmd_done;

	for(int i = 0; i < 10; i++){
		observation.target_x[i] = sim.target_x[i];
		observation.target_y[i] = sim.target_y[i];
	}
	for(int i = 0; i < 4; i++){
		observation.obstacle_x[i] = sim.obstacle_x[i];
		observation.obstacle_y[i] = sim.obstacle_y[i];
	}

	this.state.update(observation);

	return true;
}
