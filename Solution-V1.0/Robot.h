#pragma once

#include <cmath>
#include "structs.h"
#include "Plank.h"


class Robot{
private:
	point_t position;
	float orientation;
	float robot_Timer;
	float speed;
	Plank current_Plank;
	
public:
	//constructors
	Robot();

	//get
	point_t getPosition();
	float getOrientation();
	float getRobotTimer();
	float getSpeed();
	float getPlank();

	void update(float x,float y,float q);
	
	bool isMoving();
};