#pragma once

#include<queue>

static const int DRONE_SPEED = 1;
static const float ROBOT_SPEED = 0.33;
static const float MATH_PI = 3.141592653589793238;


struct point_t{
	float x;
	float y;
	float z;
	int travel_Time;
};

point_t point_Zero = {.x = 0.0, .y = 0.0, .z = 0.0, .travel_Time = 0.0};

struct bounds_t{
	int x_Max;
	int y_Max;
};

struct plank_t
{
    point_t first;
    point_t last;
    float length;
	bool will_Exit_Green;
	bool will_Exit_Red;
};

enum drone_State_t
{
    drone_landingOnTop,
    drone_landingInFront,
    drone_waiting,
    drone_tracking,
	drone_chooseAction,
	drone_chooseTarget,
	drone_start,
	drone_waitForAction,
	drone_noTargetFound,
	drone_terminate
};

enum action_Type_t
{
    actionType_NoCommand = 0,   // continue doing whatever you are doing
    actionType_LandOnTopOf,     // trigger one 45 deg turn of robot (i)
    actionType_LandInFrontOf,   // trigger one 180 deg turn of robot (i)
    actionType_LandAtPoint,	 // land at a given point
    actionType_Track,           // follow robot (i) at a constant height
    actionType_Search           // ascend to 3 meters and go to (x, y)
};

struct observation_t
{
    float elapsed_time;
    float drone_x;
    float drone_y;
    bool  drone_cmd_done;

    float target_x[10];
    float target_y[10];
    float target_q[10];

    float obstacle_x[4];
    float obstacle_y[4];
    float obstacle_q[4];
};


struct action_t{
    action_Type_t type;
    // Robot target;
    point_t waypoints[10];
    float reward;
    float when_To_Act;
};


struct tree_action_t{
    std::queue<action_t> actions;
    float reward;
};
