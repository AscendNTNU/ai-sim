#include "Drone.h"

Drone::Drone(){
	position = point_Zero;
	orientation = 0
	angle_Of_Motion = 0;
	speed = DRONE_SPEED;
	drone_State = starting;
}

point_t Drone::getPosition(){
	return this.position;
}

drone_State_t Drone::getState(){
	return this.state;
}

bool Drone::update(observation_t){
	
	point_t new_Position = {observation.drone_x, observation.drone_y, 1}
	this.prev_Position = this.position;

	this.position = new_Position;
	this.command_Done = observation.drone_cmd_done;
}


void Drone::wait(float time){
	//NOT IMPLEMENTED YET
}

int Drone::doAction(action_t action){

	//Simulaiton implementation
	sim_Command cmd;
    cmd.type = action.type;
    cmd.i = action.target.index;
    cmd.reward = action.reward;
    sim_send_cmd(&action.cmd);
}

float Drone::getDistanceToPoint(point_t point){

	float x_Distance = point.x - this.position.x;
	float y_Distance = point.y - this.position.y;

	return sqrt(pow(x_Distance,2) + pow(y_Distance,2))
}


IntersectionPoint Drone::getInterceptPoint(Robot robot) {
	
	Drone drone = new Drone(); 

	if(time_Until_Turn > 18) {
		float time_Since_Turn = 20-robot.time_Until_Turn;
		robot.angle = robot.angle - (MATH_PI/2)*(time_Since_Turn) + MATH_PI;

		drone.angle_Of_Motion = atan2(robot.position.y-drone.position.y, robot.position.x-drone.position.x);
		drone.position.x = this.position.x + (2-time_Since_Turn)*drone.speed*cos(drone.angle_of_Motion);
		drone.position.y = this.position.y + (2-time_Since_Turn)*drone.speed*sin(drone.angle_of_Motion);
	}
	
	//Math to calculate if direct
	float a = robot.position.x; float b = robot.speed; float c = robot.angle; float d = robot.position.y; float e = drone.position.x; float f = drone.position.y; float g = drone.speed;
	float ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
	float tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));

	float t1 = (std::max)(time_a, time_b);
	float t2 = 0;

	float x_bf = 0;
	float y_bf = 0;

	if(time1 > (time_UntilTurn+2))
	{
		time1 = time_Until_Turn;
		float x_b1 = robot.position.x+robot.time_Until_Turn*robot.speed*cos(robot.angle);
		float y_b1 = robot.position.y+robot.time_Until_Turn*robot.speed*sin(robot.angle);
		float angleDrone1 = atan2(y_b1-drone.position.y, x_b1-drone.position.x);

		float a = x_b1; float b = v_b; float c = robot.angle+MATH_PI; float d = y_b1; float e = drone.position.x + (robot.time_Until_Turn+2)*drone.speed*cos(angleDrone1); float f = drone.position.y + (robot.time_Until_Turn+2)*drone.speed*sin(angleDrone1); float g = drone.speed;
		ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		t2 = (std::max)(ta, tb);

		float x_d1 = e;
		float y_d1 = f;
		
		x_bf = x_b1+t2*robot.speed*cos(robot.angle+MATH_PI);
		y_bf = y_b1+t2*robot.speed*sin(robot.angle+MATH_PI);

		float angleDrone2 = atan2(y_bf-y_d1, x_bf-x_d1);
	}
	else
	{
		x_bf = robot.position.x+t1*robot.speed*cos(robot.angle);
		y_bf = robot.position.y+t1*robot.speed*sin(robot.angle);
		float angleDrone1 = atan2(y_bf-drone.position.y, x_bf-drone.position.x);
	}
	IntersectionPoint intersection;
	intersection.x = x_bf;
	intersection.y = y_bf;
	std::cout << "T1: " << t1 << std::endl;
	std::cout << "T2: " << t2 << std::endl;
	float t = t1+t2;
	intersection.travel_time = t;

    // Need to check if intersectionPoint is outside of grid
	return intersection;
}