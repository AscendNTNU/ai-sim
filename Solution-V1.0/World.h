#pragma once

#include "structs.h"
#include <time.h>

class World{
private:
	point_t origin;
	time_t start_Time;
	float orientation;
	bounds_t bounds;

public:
	//Constructors
	World(float orientation);

	//Get
	point_t getOrigin();
	double getCurrentTime();
	float getOrientation();
	bounds_t getBounds();
	float getGridValue(float X, float Y);
	bool startTimer();
};