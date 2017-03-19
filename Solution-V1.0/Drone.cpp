#include "Drone.h"

point_t getPosition(){
	return this.position;
}

drone_State_t getState(){
	return this.state;
}

void wait(float time){
	//NOT IMPLEMENTED YET
}

int doAction(action_t action){

	//Simulaiton implementation
	sim_Command cmd;
    cmd.type = action.type;
    cmd.i = action.target.index;
    cmd.reward = action.reward;
    sim_send_cmd(&action.cmd);
}

float getDistanceToPoint(point_t point){

	float x_Distance = point.x - this.position.x;
	float y_Distance = point.y - this.position.y;

	return sqrt(pow(x_Distance,2) + pow(y_Distance,2))
}


IntersectionPoint getInterceptPointWithTurn(float x_b0, float y_b0, float th_b, float v_b, float tTilTurn, float x_d, float y_d, float v_d) {
	if(tTilTurn > 18) {
		float tSinceTurn = 20-tTilTurn;
		//no idea, but not normal, since the robot is turning
		//th_f = th_curr - th_rotSoFar (rot/s * s) + 180 degrees
		th_b   = th_b    - (MATH_PI/2)*(tSinceTurn)   + MATH_PI;

		float th_fly = atan2(y_b0-y_d, x_b0-x_d);
		x_d = x_d + (2-tSinceTurn)*v_d*cos(th_fly);
		y_d = y_d + (2-tSinceTurn)*v_d*sin(th_fly);
	}
	
	//Math to calculate if direct
	float a = x_b0; float b = v_b; float c = th_b; float d = y_b0; float e = x_d; float f = y_d; float g = v_d;
	float ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
	float tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));

	float t1 = (std::max)(ta, tb);
	float t2 = 0;

	float x_bf = 0;
	float y_bf = 0;

	if(t1 > (tTilTurn+2))
	{
		t1 = tTilTurn;
		float x_b1 = x_b0+tTilTurn*v_b*cos(th_b);
		float y_b1 = y_b0+tTilTurn*v_b*sin(th_b);
		float angleDrone1 = atan2(y_b1-y_d, x_b1-x_d);

		float a = x_b0 + tTilTurn*v_b*cos(th_b); float b = v_b; float c = th_b+MATH_PI; float d = y_b0 + tTilTurn*v_b*sin(th_b); float e = x_d + (tTilTurn+2)*v_d*cos(angleDrone1); float f = y_d + (tTilTurn+2)*v_d*sin(angleDrone1); float g = v_d;
		ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		t2 = (std::max)(ta, tb);

		float x_d1 = e;
		float y_d1 = f;
		
		x_bf = x_b1+t2*v_b*cos(th_b+MATH_PI);
		y_bf = y_b1+t2*v_b*sin(th_b+MATH_PI);

		float angleDrone2 = atan2(y_bf-y_d1, x_bf-x_d1);
	}
	else
	{
		x_bf = x_b0+t1*v_b*cos(th_b);
		y_bf = y_b0+t1*v_b*sin(th_b);
		float angleDrone1 = atan2(y_bf-y_d, x_bf-x_d);
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


point_t getIntersectionPoint(Robot robot){
	if(robot.time_Until_Turn > robot.DRIVE_TIME){		//If robot 
		float tSinceTurn = 20-tTilTurn;
		//no idea, but not normal, since the robot is turning
		//th_f = th_curr - th_rotSoFar (rot/s * s) + 180 degrees
		th_b   = th_b    - (MATH_PI/2)*(tSinceTurn)   + MATH_PI;

		float th_fly = atan2(y_b0-y_d, x_b0-x_d);
		x_d = x_d + (2-tSinceTurn)*v_d*cos(th_fly);
		y_d = y_d + (2-tSinceTurn)*v_d*sin(th_fly);
	}
	
	//Math to calculate if direct
	float a = x_b0; float b = v_b; float c = th_b; float d = y_b0; float e = x_d; float f = y_d; float g = v_d;
	float ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
	float tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));

	float t1 = (std::max)(ta, tb);
	float t2 = 0;

	float x_bf = 0;
	float y_bf = 0;

	if(t1 > (tTilTurn+2))
	{
		t1 = tTilTurn;
		float x_b1 = x_b0+tTilTurn*v_b*cos(th_b);
		float y_b1 = y_b0+tTilTurn*v_b*sin(th_b);
		float angleDrone1 = atan2(y_b1-y_d, x_b1-x_d);

		float a = x_b0 + tTilTurn*v_b*cos(th_b); float b = v_b; float c = th_b+MATH_PI; float d = y_b0 + tTilTurn*v_b*sin(th_b); float e = x_d + (tTilTurn+2)*v_d*cos(angleDrone1); float f = y_d + (tTilTurn+2)*v_d*sin(angleDrone1); float g = v_d;
		ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		t2 = (std::max)(ta, tb);

		float x_d1 = e;
		float y_d1 = f;
		
		x_bf = x_b1+t2*v_b*cos(th_b+MATH_PI);
		y_bf = y_b1+t2*v_b*sin(th_b+MATH_PI);

		float angleDrone2 = atan2(y_bf-y_d1, x_bf-x_d1);
	}
	else
	{
		x_bf = x_b0+t1*v_b*cos(th_b);
		y_bf = y_b0+t1*v_b*sin(th_b);
		float angleDrone1 = atan2(y_bf-y_d, x_bf-x_d);
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