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

float World::gridValue(float X, float Y){
    float value = (-9.995004e+02)+(9.976812e+01)*X+(-1.004701e+02)*Y
        +(-5.785388e+01)*pow(X,2)+(1.161562e+01)*X*Y+(5.477725e+01)*pow(Y,2)
        +(1.260229e+01)*pow(X,3)+(1.299816e+01)*pow(X,2)*Y+(-1.438667e+01)*X*pow(Y,2)+(-1.158062e+01)*pow(Y,3)
        +(-1.404096e+00)*pow(X,4)+(-3.106303e+00)*pow(X,3)*Y+(4.263504e-01)*pow(X,2)*pow(Y,2)
        +(2.851553e+00)*X*pow(Y,3)+(1.301842e+00)*pow(Y,4)
        +(9.053408e-02)*pow(X,5)+(2.901147e-01)*pow(X,4)*Y+(1.327346e-01)*pow(X,3)*pow(Y,2)
        +(-1.761180e-01)*pow(X,2)*pow(Y,3)+(-2.603853e-01)*X*pow(Y,4)+(-8.415694e-02)*pow(Y,5)
        +(-3.615309e-03)*pow(X,6)+(-1.235169e-02)*pow(X,5)*Y+(-1.602868e-02)*pow(X,4)*pow(Y,2)
        +(3.840976e-03)*pow(X,3)*pow(Y,3)+(1.239923e-02)*pow(X,2)*pow(Y,4)
        +(1.283802e-02)*X*pow(Y,5)+(3.201336e-03)*pow(Y,6)
        +(8.890888e-05)*pow(X,7)+(1.960570e-04)*pow(X,6)*Y+(7.353331e-04)*pow(X,5)*pow(Y,2)
        +(-9.145182e-05)*pow(X,4)*pow(Y,3)+(8.794847e-10)*pow(X,3)*pow(Y,4)
        +(-6.113303e-04)*pow(X,2)*pow(Y,5)+(-2.451141e-04)*X*pow(Y,6)+(-7.627948e-05)*pow(Y,7)
        +(-1.058445e-06)*pow(X,8)+(4.059809e-11)*pow(X,7)*Y+(-1.167195e-05)*pow(X,6)*pow(Y,2)
        +(-4.630460e-12)*pow(X,5)*pow(Y,3)+(-1.355465e-11)*pow(X,4)*pow(Y,4)
        +(-5.731993e-12)*pow(X,3)*pow(Y,5)+(1.167198e-05)*pow(X,2)*pow(Y,6)
        +(3.539047e-11)*X*pow(Y,7)+(1.058675e-06)*pow(Y,8);

    return value;
}
