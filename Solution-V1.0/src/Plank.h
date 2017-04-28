#pragma once

#include <iostream>

#include "structs.h"
#include "World.h"

extern World* world;

class Plank{
private:
    point_t endpoint_1; //First enpoint robot meets
    point_t endpoint_2; //Second endpoint robot meets
    float length;
    float reward;
    float angle;
public:
	Plank();
	Plank(point_t position, float angle, float time_After_Turn, int num_Iterations);
	float getReward();
	float getAngle();
	float getLength();
	
	bool willExitGreen();
	bool willExitRed();
	float calculateReward(int n);
	void updatePlank(point_t position, float angle, float time_After_Turn, int num_Iterations);
	bool pointIsOutsideOfPlank(point_t point);
};