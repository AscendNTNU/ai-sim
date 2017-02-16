#define SIM_IMPLEMENTATION
#define SIM_CLIENT_CODE
#include "../sim.h"
#include "../gui.h"
#include <stdio.h>
#include <iostream>

static double SPEED = 0.33;
static double GRID[22][22];
static double M_PI = 3.141592653589793238;

struct Plank
{
    float x_1;
    float y_1;
    float x_2;
    float y_2;
    float length;
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
};

enum ai_State
{
    ai_landingOnTop,
    ai_landingInFront,
    ai_waiting,
    ai_tracking
};

struct ActionReward
{
    ai_State action;
    float reward;
    float x;
    float y;
};

void createGrid(){
    float grid[22][22];
    for(int x = 0; x < 22; x++) {
        for (int y = 0; y < 22; y++) {
            grid[x][y] = 0;
            if (x == 21 || x == 0 || y == 0) { grid[x][y] = -1000.0; }
            if (y == 21){
                grid[x][y] = 2000.0;
            }
        }
    }
    //Setter indre verdier
    for (int n = 0; n < 5; n++) {
        //Starter i (1,1)
        for (int x = 1; x < 21; x++){
            for (int y = 1; y < 21; y++){
                float snitt = (grid[x + 1][y] + grid[x - 1][y] + grid[x][y + 1] + grid[x][y - 1]) / 4;
                grid[x][y] = snitt;
            }
        }
        //Starter i (1,20)
        for (int x = 1; x < 21; x++){
            for (int y = 20; y > 0; y--){
                float snitt = (grid[x + 1][y] + grid[x - 1][y] + grid[x][y + 1] + grid[x][y - 1]) / 4;
                grid[x][y] = snitt;
            }
        }
        // Starter i (20,1)
        for (int x = 20; x > 0; x--){
            for (int y = 1; y < 21; y++){
                float snitt = (grid[x + 1][y] + grid[x - 1][y] + grid[x][y + 1] + grid[x][y - 1]) / 4;
                grid[x][y] = snitt;
            }
        }
        //Starter i (20,20)
        for (int x = 20; x > 0; x--){
            for (int y = 20; y > 0; y--){
                float snitt = (grid[x + 1][y] + grid[x - 1][y] + grid[x][y + 1] + grid[x][y - 1]) / 4;
                grid[x][y] = snitt;
            }
        }
    }
    for(int i=0; i < 22; i++){

        for(int j=0; j < 22; j++){
            std::cout << grid[i][j] << " ";
            GRID[i][j] = grid[i][j]+1000;
        }
        std::cout << std::endl;
    }
}


float gridValue(float x, float y)
{
    if (y>20) {
        return 2000;
    } else if (y < 0 || x < 0 || x > 20) {
        return 0;
    }
    y = abs(20-y);
    int x_0 = 10; //The peak's x-position
    int y_0 = 20; //The peak's y-position
    float y_v = 10; // The spread in y-direction
    float x_v = 7; // The spread in x-direction
    int amplitude = 100; // How "extreme" the values are
    float value = amplitude*exp(-((pow(x - x_0, 2) / (2 * pow(x_v, 2))) - ((pow(y - y_0, 2)) / (2 * pow(y_v, 2)))));
    return value;
}

void printValueGrid(){
    float grid[22][22];
    for(int x = 0; x < 22; x++) {
        for (int y = 0; y < 22; y++) {
            grid[x][y] = gridValue(x,y);
            std::cout << grid[x][y] << " ";
        }
        std::cout << std::endl;
    }
}

int check_ifInArena(float x){
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

Plank createPlank(float x, float y, float theta, int timeToTurn)
{

    Plank plank;
    plank.x_1 = check_ifInArena(timeToTurn*SPEED*cos(theta) + x);
    plank.y_1 = check_ifInArena(timeToTurn*SPEED*sin(theta) + y);
    plank.x_2 = check_ifInArena((timeToTurn - 20)*SPEED*cos(theta) + x);
    plank.y_2 = check_ifInArena((timeToTurn - 20)*SPEED*sin(theta) + y);
    
    float dx = plank.x_2 - plank.x_1;
    float dy = plank.y_2 - plank.y_1;
    plank.length = sqrt(dx*dx + dy*dy);

    return plank;
}

float getPlankValue(float (*f)(float x, float y), Plank plank, int n){

    float step_x = (plank.x_1 - plank.x_2)/ n;  // width of each small rectangle
    float step_y = (plank.y_1 - plank.y_2)/ n;  // width of each small rectangle
    float area = 0.0;  // signed area

    std::cout << "Plank length" << plank.length << std::endl;

    for (int i = 0; i < n; i++) {
        area += f(plank.x_1 + (i + 0.5) * step_x, plank.y_1 + (i + 0.5) * step_y) * plank.length; // sum up each small rectangle
    }
    area = area/plank.length;
    return area;
}

float findRobotValue(float x_robot, float y_robot, float theta, int timeToTurn)
{
    float reward1 = 0;
    float reward2 = 0;
    Plank positions = createPlank(x_robot, y_robot, theta, timeToTurn);
    // reward1 = gridValue(positions.x_1, positions.y_1);
    // reward2 = gridValue(positions.x_2, positions.y_2);
    reward1 = GRID[(int)positions.x_1][(int)positions.y_1];
    reward2 = GRID[(int)positions.x_2][(int)positions.y_2];

    //if(reward1 == gridValue(0,0) || reward1 == gridValue(20,20)){
    if(reward1 == GRID[0][21] || reward1 == GRID[0][0]){
        return 2*reward1;
    }
    //else if(reward2 == gridValue(0,0) || reward2 == gridValue(20,20)){
    if(reward1 == GRID[0][21] || reward1 == GRID[0][0]){
        return 2*reward2;
    }
    else{
        return reward1-reward2;
    }

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
        moving = true;
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
    double rotationAngle = atan2(Ay-Cy, Ax-Cx) - M_PI/2;
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
IntersectionPoint getInterceptPointWithTurn(double x_b0, double y_b0, double th_b, double v_b, double tTilTurn, double x_d, double y_d, double v_d) {
	if(tTilTurn > 18) {
		float tSinceTurn = 20-tTilTurn;
		//no idea, but not normal, since the robot is turning
		//th_f = th_curr - th_rotSoFar (rot/s * s) + 180 degrees
		th_b   = th_b    - (M_PI/2)*(tSinceTurn)   + M_PI;

		float th_fly = atan2(y_b0-y_d, x_b0-x_d);
		x_d = x_d + (2-tSinceTurn)*v_d*cos(th_fly);
		y_d = y_d + (2-tSinceTurn)*v_d*sin(th_fly);
	}
	
	//Math to calculate if direct
	double a = x_b0; double b = v_b; double c = th_b; double d = y_b0; double e = x_d; double f = y_d; double g = v_d;
	double ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
	double tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));

	double t1 = max(ta, tb);
	double t2 = 0;

	double x_bf = 0;
	double y_bf = 0;

	if(t1 > (tTilTurn+2))
	{
		t1 = tTilTurn;
		double x_b1 = x_b0+tTilTurn*v_b*cos(th_b);
		double y_b1 = y_b0+tTilTurn*v_b*sin(th_b);
		double angleDrone1 = atan2(y_b1-y_d, x_b1-x_d);

		double a = x_b0 + tTilTurn*v_b*cos(th_b); double b = v_b; double c = th_b+M_PI; double d = y_b0 + tTilTurn*v_b*sin(th_b); double e = x_d + (tTilTurn+2)*v_d*cos(angleDrone1); double f = y_d + (tTilTurn+2)*v_d*sin(angleDrone1); double g = v_d;
		ta =(-sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		tb = (sqrt(pow(b,2)*pow(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c),2) - 4*(-pow(a,2) + 2*a*e - pow(d,2) + 2*d*f - pow(e,2) - pow(f,2))*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2))) - b*(-2*a*cos(c) - 2*d*sin(c) + 2*e*cos(c) + 2*f*sin(c)))/(2*(-pow(b,2)*pow(sin(c),2) - pow(b,2)*pow(cos(c),2) + pow(g,2)));
		t2 = max(ta, tb);

		double x_d1 = e;
		double y_d1 = f;
		
		x_bf = x_b1+t2*v_b*cos(th_b+M_PI);
		y_bf = y_b1+t2*v_b*sin(th_b+M_PI);

		double angleDrone2 = atan2(y_bf-y_d1, x_bf-x_d1);
	}
	else
	{
		x_bf = x_b0+t1*v_b*cos(th_b);
		y_bf = y_b0+t1*v_b*sin(th_b);
		double angleDrone1 = atan2(y_bf-y_d, x_bf-x_d);
	}
	IntersectionPoint intersection;
	intersection.x = x_bf;
	intersection.y = y_bf;
	double t = t1+t2;
	intersection.travel_time = t;
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
    int max_value = 0;
    int temp_value = 0;
    int index = 0;
    Target target;

    for(int i = 0; i < Num_Targets; i++){
        if(!observed_state.target_removed[i]){
            if (!targetIsMoving(i, previous_state, observed_state))
            {
                std::cout << "Target not moving" << std::endl;
                continue;
            }

            float angle = wrap_angle(observed_state.target_q[i]);

            Plank plank = createPlank(observed_state.target_x[i], observed_state.target_y[i],
                angle, (int)observed_state.elapsed_time % 20);

            temp_value = getPlankValue(gridValue, plank, 5);
            // temp_value = GRID[(int)observed_state.target_x[i]][(int)observed_state.target_y[i]];

            if(temp_value > max_value){
                max_value = temp_value;
                target.index = i;
                target.plank = plank;
                target.angle = wrap_angle(observed_state.target_q[i]);
            }
        }
    }
    return target;
}

IntersectionPoint calculateInterceptionPoint(sim_Observed_State state, Target target){
    IntersectionPoint intersection;
    //calculate future x and y values to robot
    //double distance = sqrt(pow(state.drone_x - state.target_x[i], 2) + pow(state.drone_y - state.target_y[i], 2));
    //double time     = distance/
    float drone_speed = Robot_Speed;
    int i = target.index;
    float angle = wrap_angle(state.target_q[i]);
    double phi = angleToIntercept(state.target_x[i], state.target_y[i], state.drone_x, state.drone_y, angle, drone_speed);
    intersection.travel_time = getTimeToIntercept(state.target_x[i], state.drone_x, state.target_q[i], drone_speed, phi);
    
    float distance = drone_speed*intersection.travel_time;

    intersection.x = state.drone_x + distance*cos(phi);
    intersection.y = state.drone_y + distance*sin(phi);

	//Added to try new function
	intersection = getInterceptPointWithTurn(state.target_x[i], state.target_y[i], state.target_q[i], .33, 20 - (int)state.elapsed_time%20 + (int)state.elapsed_time - state.elapsed_time, state.drone_x, state.drone_y, 1);

    return intersection;
}

ActionReward getBestActionAtPoint(Target target, sim_Observed_State state) {
    ActionReward action_reward;
    int rewardOnTop = findRobotValue(target.intersection.x, target.intersection.y,
            wrap_angle(target.angle + 0.785), (int)(state.elapsed_time+target.intersection.travel_time) % 20); //0.785 radians is almost 45 degerees

    int rewardInFront = findRobotValue(target.intersection.x, target.intersection.y,
            wrap_angle(target.angle + 3.14), (int)(state.elapsed_time+target.intersection.travel_time) % 20);

    int rewardForWait = findRobotValue(target.intersection.x, target.intersection.y,
            target.angle, (int)(state.elapsed_time+target.intersection.travel_time) % 20);
    std::cout << "Reward in front " << rewardInFront << std::endl;
    std::cout << "Reward on top " << rewardOnTop << std::endl;
    std::cout << "Reward wait" << rewardForWait << std::endl;

    int max_reward = max(max(rewardInFront,rewardOnTop), rewardForWait);
    if(max_reward == rewardForWait){
        action_reward.action = ai_waiting;
        action_reward.reward = rewardForWait;
    }
    else if(max_reward == rewardInFront){
        action_reward.action = ai_landingInFront;
        action_reward.reward = rewardInFront;
    }
    else if(max_reward == rewardOnTop){
        action_reward.action = ai_landingOnTop;
        action_reward.reward = rewardOnTop;
    }
    else{
        action_reward.action = ai_waiting;
        action_reward.reward = rewardForWait;
    }
    return action_reward;
}

ActionReward choose_action(sim_Observed_State state, Target target){
    int index = target.index;
    float angle = wrap_angle(state.target_q[index]);

    target.intersection = calculateInterceptionPoint(state, target);
	std::cout << "Intersection point: " << target.intersection.x << ", " << target.intersection.y << std::endl;


    float n = 10;
    float step_size = target.plank.length/n;
    float step_x = step_size*cos(angle);
    float step_y = step_size*sin(angle);

    float x = target.intersection.x;
    float y = target.intersection.y;
    int time_to_turn = ((int)state.elapsed_time % 20) - target.intersection.travel_time;

    // Temporary max rewarded action
    ActionReward best_action;
    best_action.reward = -1000.0;
    best_action.action = ai_waiting;
    ActionReward action_to_check;

    for (int i = 1; i <= n; i++)
    {
        //if (time_to_turn <= 0) {
            //std::cout << "Time ran out before end of plank was reached " << std::endl;
            //break;
        //}
        //if (x == target.plank.x_1 && y == target.plank.y_1) {
            //std::cout << "End of plank was reached ";
            //break;
        //}

        action_to_check = getBestActionAtPoint(target, state);

        if (action_to_check.reward > best_action.reward) {
            best_action = action_to_check;
            best_action.x = x;
            best_action.y = y;
        }

        x = x+step_x;
        y = y+step_y;
        time_to_turn = time_to_turn - Robot_Speed/(step_size*1000); // Multiplied by 1000 to get Milimeters from Meters
    }

    return best_action;
}

int main()
{
    createGrid(); 
    printValueGrid();
    sim_init_msgs(true);

    ActionReward action_pos_reward;
    ai_State ai_state = ai_waiting;

    sim_State state;
    bool running = true;

    sim_Command cmd;
    sim_Observed_State observed_state;
    sim_Observed_State previous_state;
    int target_index = -1;
    float last_action_time;

    Target target;

    while (running)
    {
        sim_recv_state(&state);
        previous_state = observed_state;
        sim_Observed_State observed_state = sim_observe_state(state);

        if (observed_state.drone_cmd_done || ai_state == ai_tracking) //&& observed_state.elapsed_time - last_action_time >= 4.0)
        {
            last_action_time = observed_state.elapsed_time;

            if(target_index == -1){
                target = choose_target(observed_state, previous_state);
                target_index = target.index; 
                //ai_state = ai_tracking;
                //cmd.type = sim_CommandType_Track;
                //cmd.i = target.index;
                std::cout << "Tracking" << std::endl;
            }

            else if(target_inActionRange(observed_state, target.index)
                    && targetIsMoving(target.index, previous_state, observed_state))
            {                
                action_pos_reward = choose_action(observed_state, target);
                ai_state = action_pos_reward.action;

                cmd.type = sim_CommandType_Search;
                cmd.x = action_pos_reward.x;
                cmd.y = action_pos_reward.y;
                sim_send_cmd(&cmd);

                while(!observed_state.drone_cmd_done);

                switch (ai_state)
                {
                    case ai_landingOnTop:
                        cmd.type = sim_CommandType_LandOnTopOf;
                        cmd.i = target.index;
                        std::cout << "Top" << std::endl;
                        sim_send_cmd(&cmd);
                    break;
                    case ai_landingInFront:
                        cmd.type = sim_CommandType_LandInFrontOf;
                        cmd.i = target.index;
                        std::cout << "Front" << std::endl;
                        sim_send_cmd(&cmd);
                    break;
                    case ai_waiting:
                        std::cout << "WAITING" << std::endl;
                    break;
                }
            target_index = -1;
            }
        }
    }

    return 0;
}
