#pragma once

#include <cmath>
#include "structs.h"
#include "Plank.h"

class Robot{
private:
	point_t position;
	float orientation;
	point_t old_Position;
	float old_Orientation;
	float time_After_Turn;
	float speed;	
public:
	//constructors
	Robot();
	Plank* current_Plank;

	//get
	point_t getPosition();
	float getOrientation();
	float getTimeAfterTurn();
	float getSpeed();
	Plank* getCurrentPlank();

	//set
	void setPositionOrientation(point_t positionw, float q);
	void addToTimer(float time);
	
	//methods
	action_t chooseAction(Robot target);
	bool isMoving();
	void update(point_t position,float q);
};