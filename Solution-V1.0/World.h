#pragma once

#include "structs.h"
#include "AI.h"
#include "SimIn.h"

class World{
private:
	point_t origin;
	float time;
	float orientation;
	bounds_t bounds;

public:

	//Constructors
	World();

	//Get
	point_t getOrigin();
	float getCurrentTime();
	float getOrientation();
	bounds_t getBounds();
	float gridValue(float X, float Y);

	//Initialize
	bool startTimer();

	//Update
	bool update_world();
};