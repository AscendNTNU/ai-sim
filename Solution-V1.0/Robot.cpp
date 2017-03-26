#include "World.h"

Robot::Robot(){
	this.position = point_Zero;
	this.orientation = 0;
	robot_Timer = 0;
	speed = 0.33;
	current_Plank = new Plank(this);
}

point_t Robot::getPosition(){
	return this.position;
}
float Robot::getOrientation(){
	return this.orientation;
}
float Robot::getTimer(){
	return this.rimer
}
float Robot::getRobotSpeed(){
	return this.speed;
}
Plank Robot::getCurrentPlank(){
	return this.current_Plank;
}

bool Robot::isMoving(){
	if (this.old_position.x == this.position.x && 
	    this.old_position.y == this.position.y) {
	    return false;
	} else {
		return true;
	}
}

void Robot::update(point_t new_Position, float new_Orientation){
	this.old_Position = this.position;
	this.old_Orientation = this.orientation;

	this.position.x = x;
	this.position.y = y;
	this.orientation = q;
}