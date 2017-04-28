#include "Drone.h"

Drone::Drone(){

	this->position = point_Zero;
	this->orientation = 0;
	this->angle_Of_Motion = 0;
	this->speed = DRONE_SPEED;
	this->state = starting;
}

point_t Drone::getPosition(){
	return this->position;
}

drone_State_t Drone::getState(){
	return this->state;
}

bool Drone::update(observation_t observation){
	
	point_t new_Position = {observation.drone_x, observation.drone_y, 1};
	this->prev_Position =  this->position;

	this->position = new_Position;
	this->command_Done = observation.drone_cmd_done;
	return true;
}


void Drone::wait(float time){
	//NOT IMPLEMENTED YET
}

int Drone::doAction(action_t action){

	//Simulaiton implementation
	// sim_Command cmd;
 //    cmd.type = action.type;
 //    cmd.i = action.target.index;
 //    cmd.reward = action.reward;
 //    sim_send_cmd(&action.cmd);
	return -1;
}

float Drone::getDistanceToPoint(point_t point){

	float x_Distance = point.x - this->position.x;
	float y_Distance = point.y - this->position.y;

	return sqrt(pow(x_Distance,2) + pow(y_Distance,2));
}


point_t Drone::getInterceptPoint(Robot* robot) {
	
	//Drone* drone = new Drone(); 
	float time_Until_Turn = 20 - robot->getTimeAfterTurn();
	float robot_Ori = robot->getOrientation();
	point_t robot_Pos = robot->getPosition();

	if(time_Until_Turn > 18) {
		float time_Since_Turn = robot->getTimeAfterTurn();
		robot_Ori = robot_Ori - (MATH_PI/2)*(time_Since_Turn) + MATH_PI;

		this->angle_Of_Motion = atan2(robot_Pos.y-this->position.y, robot_Pos.x-this->position.x);
		this->position.x = this->position.x + (2-time_Since_Turn)*this->speed*cos(this->angle_Of_Motion);
		this->position.y = this->position.y + (2-time_Since_Turn)*this->speed*sin(this->angle_Of_Motion);
	}
	
	//Math to calculate if direct
	float a = robot_Pos.x; float b = robot->getSpeed(); float c = robot_Ori; float d = robot_Pos.y; float e = this->position.x; float f = this->position.y; float g = this->speed;
	float ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
	float tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));

	float t1 = (std::max)(ta, tb);
	float t2 = 0;

	float x_bf = 0;
	float y_bf = 0;

	if(t1 > (time_Until_Turn+2))
	{
		t1 = time_Until_Turn;
		float x_b1 = robot_Pos.x +time_Until_Turn*robot->getSpeed()*cos(robot_Ori);
		float y_b1 = robot_Pos.y +time_Until_Turn*robot->getSpeed()*sin(robot_Ori);
		float angleDrone1 = atan2(y_b1-this->position.y, x_b1-this->position.x);

		float a = x_b1; float b = robot->getSpeed(); float c = robot_Ori+MATH_PI; float d = y_b1; float e = this->position.x + (time_Until_Turn+2)*this->speed*cos(angleDrone1); float f = this->position.y + (time_Until_Turn+2)*this->speed*sin(angleDrone1); float g = this->speed;
		ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		t2 = (std::max)(ta, tb);

		float x_d1 = e;
		float y_d1 = f;
		
		x_bf = x_b1+t2*robot->getSpeed()*cos(robot_Ori+MATH_PI);
		y_bf = y_b1+t2*robot->getSpeed()*sin(robot_Ori+MATH_PI);

		float angleDrone2 = atan2(y_bf-y_d1, x_bf-x_d1);
	}
	else
	{
		x_bf = robot_Pos.x+t1*robot->getSpeed()*cos(robot_Ori);
		y_bf = robot_Pos.y+t1*robot->getSpeed()*sin(robot_Ori);
		float angleDrone1 = atan2(y_bf-this->position.y, x_bf-this->position.x);
	}
	point_t intersection;
	intersection.x = x_bf;
	intersection.y = y_bf;
	std::cout << "T1: " << t1 << std::endl;
	std::cout << "T2: " << t2 << std::endl;
	float t = t1+t2;
	intersection.travel_Time = t;

    // Need to check if intersectionPoint is outside of grid
    // point_t intersection = point_Zero;
    // std::cout << "Intersection implementation not implemented yet" << std::endl;
	return intersection;
}