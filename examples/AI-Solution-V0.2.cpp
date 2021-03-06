#define SIM_IMPLEMENTATION
#define SIM_CLIENT_CODE
#include "../sim.h"
#include "../gui.h"
#include <stdio.h>
#include <iostream>
#include <algorithm>

static double SPEED = 0.33;
static double MATH_PI = 3.141592653589793238;

struct Plank
{
    float x_1;
    float y_1;
    float x_2;
    float y_2;
    float length;
	bool goingOutGreen;
	bool crossesRed;
};

struct IntersectionPoint
{
    float x;
    float y;
    float travel_time;
};

struct Target
{
    Plank plank;
    IntersectionPoint intersection;
    int index;
    float angle;
    float currentReward;
};

enum ai_State
{
    ai_landingOnTop,
    ai_landingInFront,
    ai_waiting,
    ai_tracking,
	ai_chooseAction,
	ai_chooseTarget,
	ai_start,
	ai_waitForAction,
	ai_noTargetFound,
	ai_terminate
};

struct ActionReward
{
    ai_State action;
    float reward;
    float x;
    float y;
    float time_after_intersection;
    float time_until_intersection;
};

float gridValue(float X, float Y)
{
     if (Y>20) {
         return 2000;
     } else if (Y < 0 || X < 0 || X > 20) {
         return -1000;
     }

    float value = (-9.995004e+02)+(9.976812e+01)*X+(-1.004701e+02)*Y
        +(-5.785388e+01)*pow(X,2)+(1.161562e+01)*X*Y+(5.477725e+01)*pow(Y,2)
        +(1.260229e+01)*pow(X,3)+(1.299816e+01)*pow(X,2)*Y+(-1.438667e+01)*X*pow(Y,2)+(-1.158062e+01)*pow(Y,3)
        +(-1.404096e+00)*pow(X,4)+(-3.106303e+00)*pow(X,3)*Y+(4.263504e-01)*pow(X,2)*pow(Y,2)
        +(2.851553e+00)*X*pow(Y,3)+(1.301842e+00)*pow(Y,4)
        +(9.053408e-02)*pow(X,5)+(2.901147e-01)*pow(X,4)*Y+(1.327346e-01)*pow(X,3)*pow(Y,2)
        +(-1.761180e-01)*pow(X,2)*pow(Y,3)+(-2.603853e-01)*X*pow(Y,4)+(-8.415694e-02)*pow(Y,5)
        +(-3.615309e-03)*pow(X,6)+(-1.235169e-02)*pow(X,5)*Y+(-1.602868e-02)*pow(X,4)*pow(Y,2)
        +(3.840976e-03)*pow(X,3)*pow(Y,3)+(1.239923e-02)*pow(X,2)*pow(Y,4)
        +(1.283802e-02)*X*pow(Y,5)+(3.201336e-03)*pow(Y,6)
        +(8.890888e-05)*pow(X,7)+(1.960570e-04)*pow(X,6)*Y+(7.353331e-04)*pow(X,5)*pow(Y,2)
        +(-9.145182e-05)*pow(X,4)*pow(Y,3)+(8.794847e-10)*pow(X,3)*pow(Y,4)
        +(-6.113303e-04)*pow(X,2)*pow(Y,5)+(-2.451141e-04)*X*pow(Y,6)+(-7.627948e-05)*pow(Y,7)
        +(-1.058445e-06)*pow(X,8)+(4.059809e-11)*pow(X,7)*Y+(-1.167195e-05)*pow(X,6)*pow(Y,2)
        +(-4.630460e-12)*pow(X,5)*pow(Y,3)+(-1.355465e-11)*pow(X,4)*pow(Y,4)
        +(-5.731993e-12)*pow(X,3)*pow(Y,5)+(1.167198e-05)*pow(X,2)*pow(Y,6)
        +(3.539047e-11)*X*pow(Y,7)+(1.058675e-06)*pow(Y,8);

    return value;
}

int movePointInsideArena(float x){
    if(x > 21){
        return 21;
    }
    else if(x<0){
        return 0;
    }
    else{
        return x;
    }
}

bool isOutsideArena(float x) {
    bool outside = true;
    if(x > 21 || x<0){
        outside = true;
    }
    return outside;
}

bool crossesRedLine(float x1, float x2, float y1, float y2) {
	// float m = (y2-y1)/(x2-x1);

	// //Red line 1 - bottom line
	// float y_redLine1 = 0;
	// float x_kryssepunkt = (y_redLine1 - y1 + m*x1)/m;
	// if((y1<y_redLine1 || y2 <y_redLine1) && 0 < x_kryssepunkt && x_kryssepunkt < 20) {
	// 	return true;
	// }

	// //Red line 2 - left side
	// float x_redLine2 = 0;
	// float y_kryssepunkt = m*(x_redLine2-x1) + y1;
	// if((x1<x_redLine2 || x2 <x_redLine2) && 0 < y_kryssepunkt && y_kryssepunkt < 20) {
	// 	return true;
	// }

	// //Red line 3 - right side
	// float x_redLine3 = 20;
	// y_kryssepunkt = m*(x_redLine3-x1) + y1;
	// if((x1>x_redLine2 || x2 > x_redLine2) && 0 < y_kryssepunkt && y_kryssepunkt < 20) {
	// 	return true;
	// }

    if (x1 < 0 || x1 > 20 || x2 < 0 || x2 > 20 || y1 < 0 || y2 < 0) {
        return true;
    }
	return false;
}

bool isOverGreenLine(float x1, float x2, float y1, float y2) {
	// float m = (y2-y1)/(x2-x1);
	// float y_greenLine = 20.5;
	// float x_kryssepunkt = (y_greenLine - y1 + m*x1)/m;

	// if(y1 > y_greenLine && 1 < x_kryssepunkt && x_kryssepunkt < 19) {
	// 	return true;
	// }
	// else if(y2 > y_greenLine && 1 < x_kryssepunkt && x_kryssepunkt < 19) {
	// 	if(crossesRedLine(x1, x2, y1, y2)) {
	// 		return false;
	// 	}
	// 	else {
	// 		return true;
	// 	}
	// }
	// else {
	// 	return false;
	// }

    if (y1 > 20 || y2 > 20) {
        return true;
    }
    return false;
}

Plank createPlank(float x, float y, float theta, float timeToTurn)
{
    Plank plank;

    plank.x_1 = timeToTurn*SPEED*cos(theta) + x;
    plank.y_1 = timeToTurn*SPEED*sin(theta) + y;
    plank.x_2 = (timeToTurn - 20+2.5)*SPEED*cos(theta) + x;
    plank.y_2 = (timeToTurn - 20+2.5)*SPEED*sin(theta) + y;
    
    float dx = plank.x_2 - plank.x_1;
    float dy = plank.y_2 - plank.y_1;
    plank.length = sqrt(dx*dx + dy*dy);


	plank.goingOutGreen = isOverGreenLine(plank.x_1, plank.x_2, plank.y_1, plank.y_2);
	plank.crossesRed    = crossesRedLine(plank.x_1, plank.x_2, plank.y_1, plank.y_2);

    return plank;
}

float getPlankValue(float (*f)(float x, float y), Plank plank, float angle, int n){


    float step_tot = plank.length/n;
    float step_x = step_tot*cos(angle);
    float step_y = step_tot*sin(angle);

    float area = 0.0;  // signed area
    float x = 0.0;
    float y = 0.0;

    // std::cout << "Plank length" << plank.length << std::endl;

    for (int i = 0; i < n; i++) {
        x = plank.x_1 + (i + 0.5) * step_x;
        y = plank.y_1 + (i + 0.5) * step_y;
        area += f(x, y) * step_tot; // sum up each small rectangle
    }
    area = area/plank.length;
    return area;
}

float findRobotValue(float x_robot, float y_robot, float theta, float timeToTurn)
{
    Plank plank = createPlank(x_robot, y_robot, theta, timeToTurn);
//<<<<<<< Updated upstream
    float reward = getPlankValue(gridValue, plank, theta, 100);
	// if(plank.crossesRed) reward = -200000;
	// if(plank.goingOutGreen) reward = 10000000;
//=======
//    float reward = getPlankValue(gridValue, plank, theta, 5);
//	if(plank.crossesRed) reward = -200000;
//	if(plank.goingOutGreen) reward = 10000000;
//>>>>>>> Stashed changes
    return reward;
}

bool target_inActionRange(sim_Observed_State observed_state, int target){
    float dx = observed_state.drone_x - observed_state.target_x[target];
    float dy = observed_state.drone_y - observed_state.target_y[target];
    if(sqrt(dx*dx + dy*dy) > Sim_Drone_Target_Proximity){
        return true;
    }
    return false;
}

bool targetIsMoving(int target, sim_Observed_State previous_state, sim_Observed_State observed_state){
    bool moving = true;
    if (previous_state.target_x[target] == observed_state.target_x[target] && 
        previous_state.target_y[target] == observed_state.target_y[target]) 
    {
        moving = false;
    }
    if (observed_state.target_removed[target]) {
        moving = false;
    }
    return moving;
}

double rotateX(double x, double y, double th) {
    return x*cos(th)+y*sin(th);
}

double rotateY(double x, double y, double th) {
    return x*-1*sin(th) + y*cos(th);
}

/*
    A - ground robot
    C - Drone
*/
double angleToIntercept(double Ax, double Ay, double Cx, double Cy, double Aangle, double Cvel) 
{
    double Avel = Robot_Speed/1000;
    double rotationAngle = atan2(Ay-Cy, Ax-Cx) - MATH_PI/2;
    double AxTrans = Ax-Cx;
    double AyTrans = Ay-Cy;
    double AxRot = rotateX(AxTrans, AyTrans, rotationAngle);
    double AyRot = rotateY(AxTrans, AyTrans, rotationAngle);
    double CxRot = 0; //rotateX(Cx, Cy, rotationAngle);
    double CyRot = 0; //rotateY(Cx, Cy, rotationAngle);
    
    double AvelX = Avel*cos(Aangle);
    double AvelY = Avel*sin(Aangle);
    //double AvelXTrans = AvelX-Cx;
    //double AvelYTrans = AvelY-Cy;
    double AvelXrot = rotateX(AvelX, AvelY, rotationAngle);
    double AvelYrot = rotateY(AvelX, AvelY, rotationAngle);

    return acos((Avel/Cvel) * AvelXrot);
}

/*
    A - ground robot
    C - Drone
*/
float getTimeToIntercept(double Cx, double Ax, float Aangle, double Cvel, double angleToFly)
{
    double Avel = Robot_Speed/1000;
    double AvelX = Avel*cos(Aangle);
    return (float)(Cx-Ax)/(AvelX*Avel - cos(angleToFly)*Cvel);
}

/*
    A - ground robot
    C - Drone
*/
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
	std::cout << "Travel Time: " << t << std::endl;
	intersection.travel_time = t;

    // Need to check if intersectionPoint is outside of grid
	return intersection;
}

/*
    A - ground robot
    C - Drone
	TODO: Finish this
*/
double getXPosIntercept(double droneX, double droneY, double flyAngle, double speed)
{
    return 0.0;
}


//Pseudo ish kode
Target choose_target(sim_Observed_State observed_state, sim_Observed_State previous_state){
    float max_value = -200000;
    float temp_value = 0;
    int index = 0;
    Target target;
	bool robotChosen = false;

    float timeToTurn = 20 - fmod(observed_state.elapsed_time,20);

    for(int i = 0; i < Num_Targets; i++){
		std::cout << "Checking target: " << i << " at (X, Y): (" << observed_state.target_x[i] << ", " << observed_state.target_y[i] << ")" << std::endl;
		if(!observed_state.target_removed[i]){
			std::cout << "Target not removed" << std::endl;
            if (!targetIsMoving(i, previous_state, observed_state))
            {
                target.index = -1;
                return target;
                //std::cout << "Target not moving" << std::endl;
            }

            float angle = wrap_angle(observed_state.target_q[i]);

            Plank plank = createPlank(observed_state.target_x[i], observed_state.target_y[i],
                angle, timeToTurn);

			if(plank.goingOutGreen) {
				std::cout << "Target already going out of green line!" << std::endl;
				continue;
			}

            temp_value = getPlankValue(gridValue, plank, angle, 5);

            if(temp_value > max_value){
                max_value = temp_value;
                target.index = i;
                target.plank = plank;
                target.angle = wrap_angle(observed_state.target_q[i]);
                target.currentReward = temp_value;
				robotChosen = true;
            }
        }
    }
	if(!robotChosen) {
		std::cout << "NO ROBOT CHOSEN! Setting index to -1." << std::endl;
		target.index = -1;
		target.plank = createPlank(observed_state.target_x[1], observed_state.target_y[1],
									wrap_angle(observed_state.target_q[1]), timeToTurn);;
		target.angle = wrap_angle(observed_state.target_q[1]);
		target.currentReward = getPlankValue(gridValue, target.plank, target.angle, 5);
	}
	std::cout << "Chose Target: " << target.index << std::endl;
	std::cout << "Target Value: " << max_value    << std::endl;
	std::cout << "Target (X, Y): " << observed_state.target_x[target.index] << ", " << observed_state.target_y[target.index] << std::endl;

    return target;
}

IntersectionPoint calculateInterceptionPoint(sim_Observed_State state, Target target){
    IntersectionPoint intersection;
    //calculate future x and y values to robot
    //double distance = sqrt(pow(state.drone_x - state.target_x[i], 2) + pow(state.drone_y - state.target_y[i], 2));
    //double time     = distance/
    //float drone_speed = Robot_Speed;
    int i = target.index;
    //float angle = wrap_angle(state.target_q[i]);
    //double phi = angleToIntercept(state.target_x[i], state.target_y[i], state.drone_x, state.drone_y, angle, drone_speed);
    //intersection.travel_time = getTimeToIntercept(state.target_x[i], state.drone_x, state.target_q[i], drone_speed, phi);
    //
    //float distance = drone_speed*intersection.travel_time;
//
    //intersection.x = state.drone_x + distance*cos(phi);
    //intersection.y = state.drone_y + distance*sin(phi);

	//Added to try new function
	std::cout << "Before calculating intersection point" << std::endl;
		
	intersection = getInterceptPointWithTurn(state.target_x[i], state.target_y[i], state.target_q[i], 
        .33, 20 - fmod(state.elapsed_time,20) + state.elapsed_time - state.elapsed_time, state.drone_x, state.drone_y, 1);
	std::cout << "After calculating intersection point" << std::endl;
	
    return intersection;
}

ActionReward getBestActionAtPoint(Target target, float x, float y , sim_Observed_State state, float time_after_intersection) {
    ActionReward action_reward;
    float rewardOnTop = findRobotValue(x, y, wrap_angle(target.angle + 0.785),//0.785 radians is almost 45 degerees 
            (int)(state.elapsed_time+target.intersection.travel_time+time_after_intersection) % 20);

    float rewardInFront = findRobotValue(x, y, wrap_angle(target.angle + 3.14), 
            (int)(state.elapsed_time+target.intersection.travel_time+time_after_intersection) % 20);

    std::cout << "Reward 180 deg  " << rewardInFront << std::endl;
    std::cout << "Reward  45 deg  " << rewardOnTop << std::endl;

    float max_reward = (std::max)(rewardInFront,rewardOnTop);
    if(max_reward == rewardInFront){
        action_reward.action = ai_landingInFront;
        action_reward.reward = rewardInFront;
    }
    else if(max_reward == rewardOnTop){
        action_reward.action = ai_landingOnTop;
        action_reward.reward = rewardOnTop;
    }
    else{
        action_reward.action = ai_waiting;
    }
    return action_reward;
}

bool isOutsideOfPlank(float x, float y, Plank plank) {
    if ((x > plank.x_1 && x > plank.x_2) || (x < plank.x_1 && x < plank.x_2) ||
        (y > plank.y_1 && y > plank.y_2) || (y < plank.y_1 && y < plank.y_2)) {
        return true;
    } else {
        return false;
    }
}

void printActionIteration(int i, Target target, float x, float y, sim_Observed_State state, float time_until_intersection, float time_after_intersection) {
    float time_to_act = state.elapsed_time + 
                      time_until_intersection + 
                      time_after_intersection;

    std::cout << std::endl << "Iteration " << i << std::endl;
    std::cout << "Plank   X = [" << target.plank.x_1 << ", " << target.plank.x_2 << "]" << std::endl;
    std::cout << "Plank   Y = [" << target.plank.y_1 << ", " << target.plank.y_2 << "]" << std::endl;
    std::cout << "Current X = "<< x << std::endl;
    std::cout << "Current Y = "<< y << std::endl;
    std::cout << "Time to act: " << time_to_act << std::endl;
    std::cout << "Global time: " << state.elapsed_time << std::endl;
    std::cout << "Time until intersection: " << time_until_intersection << std::endl;
    std::cout << "Time after intersection: " << time_after_intersection << std::endl;
}

ActionReward choose_action(sim_Observed_State state, Target target){
    int index = target.index;
	std::cout << "Before wrap angle, index: " << target.index << std::endl;
	
    float angle = wrap_angle(state.target_q[index]);
	std::cout << "After wrap angle" << std::endl;
	
    target.intersection = calculateInterceptionPoint(state, target);
	std::cout << "Intersection point: " << target.intersection.x << ", " << target.intersection.y << std::endl;
	float temp = target.intersection.travel_time;
    //target.intersection.travel_time = 0;

    float n = 10;
    float step_size = target.plank.length/n;
    float step_x = step_size*cos(angle);
    float step_y = step_size*sin(angle);

    float x = target.intersection.x;
    float y = target.intersection.y;
    float time_after_intersection = 0;

    // Temporary max rewarded action
    ActionReward best_action;
    best_action.reward = -1000;
    best_action.action = ai_waiting;
    best_action.time_until_intersection = target.intersection.travel_time;
    
    ActionReward action_to_check;
    bool backwards = false;
    int i = 1;
    while (i > 0) {
		printActionIteration(i, target, x, y, state, best_action.time_until_intersection, time_after_intersection);

        if (isOutsideOfPlank(x,y, target.plank)) {
            std::cout << "End of plank was reached " << std::endl;
            if (backwards) {
                return best_action;
            } else {
                i = n+1;
                backwards = true;
                target.angle += MATH_PI;
            }
        }
        action_to_check = getBestActionAtPoint(target, x, y, state, time_after_intersection);

        if (action_to_check.reward > best_action.reward) {
            best_action = action_to_check;
            best_action.x = x;
            best_action.y = y;
            best_action.time_after_intersection = time_after_intersection;
            best_action.time_until_intersection = target.intersection.travel_time;
        }

        

        if (backwards) {
            x = x-step_x;
            y = y-step_y;
            i -= 1;
        } else {
            x = x+step_x;
            y = y+step_y;
            i += 1;
        }
        time_after_intersection = time_after_intersection + (step_size)/Robot_Speed;

    }
	best_action.time_until_intersection = temp;
    return best_action;
}

int main()
{
    sim_init_msgs(true);

    ActionReward action_pos_reward;
	action_pos_reward.action = ai_waiting;
    ai_State ai_state = ai_chooseTarget;

    sim_State state;
    bool running = true;

    sim_Command cmd;
    sim_Observed_State observed_state;
    sim_Observed_State previous_state;
    int target_index = -1;
    float last_action_time;
	float time_to_act = 200.0;

    Target target;

    while (running)
    {
        sim_recv_state(&state);
        previous_state = observed_state;
        sim_Observed_State observed_state = sim_observe_state(state);
		if(observed_state.elapsed_time > 600) {
			break;
		}
        
            last_action_time = observed_state.elapsed_time;

            //if(target_index == -1){
                //target = choose_target(observed_state, previous_state);
                //target_index = target.index; 
                ////ai_state = ai_tracking;
                ////cmd.type = sim_CommandType_Track;
                ////cmd.i = target.index;
                //std::cout << "Tracking" << std::endl;
            //}
//
            //else if(target_inActionRange(observed_state, target.index)
                    //&& targetIsMoving(target.index, previous_state, observed_state))
            //{                
                

            switch (ai_state)
            {
				case ai_chooseTarget:
					target = choose_target(observed_state, previous_state);
					target_index = target.index;
					std::cout << "Choose target" << std::endl;
					if(target.index == -1) {
						ai_state = ai_noTargetFound;
					}
					else {
						ai_state = ai_chooseAction;
					}
				break;
				case ai_noTargetFound:
				{
					//for sim
					int teller = 0;
					for(int i = 0; i++; i < Num_Targets) {
						if(observed_state.target_removed[i]) {
							teller++;
						}
					}
					if(teller == Num_Targets) {
						ai_state = ai_terminate;
					}
					else {
						ai_state = ai_chooseTarget;
					}
				}	
				break;
				case ai_chooseAction:
					std::cout << "In state Choose Action" << std::endl;
						
					action_pos_reward = choose_action(observed_state, target);
					ai_state = ai_waiting;
					if(action_pos_reward.action == ai_landingInFront) {
						std::cout << "Choose Action: Land in Front" << std::endl;
					}
					else if(action_pos_reward.action == ai_landingOnTop) {
						std::cout << "Choose Action: Land on top" << std::endl;
					}
					else if(action_pos_reward.action == ai_waiting) {
						std::cout << "Choose Action: Waiting" << std::endl;
						ai_state = ai_chooseTarget;
						break;
					}
					else {
						std::cout << "Choose Action: ... erm, what?" << std::endl;
					}

					cmd.type = sim_CommandType_Search;
					cmd.x = action_pos_reward.x;
					cmd.y = action_pos_reward.y;
                    cmd.reward = action_pos_reward.reward;
					sim_send_cmd(&cmd);
					// Tell drone do do action at time
					time_to_act = observed_state.elapsed_time + 
										action_pos_reward.time_until_intersection +
										action_pos_reward.time_after_intersection;

                    //previous_state = observed_state;

                    //while(!observed_state.drone_cmd_done) {
                        //observed_state = sim_observe_state(state);
                    //}

				break;
                case ai_landingOnTop:
					std::cout << "Land On Top" << std::endl;
                    cmd.type = sim_CommandType_LandOnTopOf;
                    cmd.i = target.index;
                    cmd.reward = action_pos_reward.reward;
                    sim_send_cmd(&cmd);
					ai_state = ai_waitForAction;
                break;
                case ai_landingInFront:
                    std::cout << "Land In Front" << std::endl;
                    cmd.type = sim_CommandType_LandInFrontOf;
                    cmd.i = target.index;
                    cmd.reward = action_pos_reward.reward;
                    sim_send_cmd(&cmd);
					ai_state = ai_waitForAction;
                break;
				case ai_waitForAction:
					if(observed_state.drone_cmd_done) {
						ai_state = ai_chooseTarget;
					}
				break;
                case ai_waiting:
					if(observed_state.target_removed[target.index]) {
						std::cout << "Target removed before we could do action. Choose target again." << std::endl;
						ai_state = ai_chooseTarget;
					}
					if(action_pos_reward.action == ai_waiting) {
						std::cout << "Drone decided that current plank is best. Choose target again." << std::endl;
						ai_state = ai_chooseTarget;
					}
                    if(observed_state.drone_cmd_done && 
							observed_state.elapsed_time >= time_to_act && 
							targetIsMoving(target.index, previous_state, observed_state)) {
						std::cout << "Do action" << std::endl;
						ai_state = action_pos_reward.action;
					}
					// if(ai_state == action_pos_reward.action){
					// 	//if robot has drifted
					// 	if(targetIsMoving(target.index, previous_state, observed_state) && (int)observed_state.elapsed_time %20 < 2) {
					// 		std::cout << "Think a robot has drifted. Choose action again" << std::endl;
					// 		ai_state = ai_chooseAction;
					// 	}
						//if(observed_state.elapsed_time > time_to_act) {
						//ai_state = action_pos_reward.action;
						//}
					// }
                break;
				case ai_terminate:
					running = false;
				break;
                default:
                break;
				
        //target_index = -1;
        }
    }
	std::cout << "Time is up!" << std::endl;
	//std::cout << "Time's up!" << std:endl;
    return 0;
}
