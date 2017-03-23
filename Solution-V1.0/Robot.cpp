#include "World.h"

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

void Robot::update(point_t new_Position, float new_Orientation,
					float new_Speed, float new_Timer){
	this.old_Position = this.position;
	this.old_Orientation = this.orientation;
	this.old_Timer = this.timer;
	this.old_Speed = this.speed;
	this.old_Plank = this.Plank;

	this.position = this.new_Position;
	this.orientation = this.new_Orientation;
	this.timer = this.new_Timer;
	this.speed = this.new_Speed
	this.Plank = this.new_Plank;
}