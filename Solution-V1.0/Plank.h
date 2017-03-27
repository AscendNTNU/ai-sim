#pragma once

#include "structs.h"
#include "World.h"

extern World world;

class Plank{
private:
    point_t endpoint_1; //First enpoint robot meets
    point_t endpoint_2; //Second endpoint robot meets
    float length;
    float reward;
    float angle;
public:
	Plank Plank(float x, float y, float angle, float time_Until_Turn, int num_Iterations);
	Plank Plank(Robot robot, int num_Iterations);
	float getReward();
	float getAngle();
	float getLength();
	
	bool willExitGreen();
	bool willExitRed();
	float calculateReward();
};