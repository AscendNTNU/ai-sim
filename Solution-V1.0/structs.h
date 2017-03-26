static const DRONE_SPEED = 1;
static const ROBOT_SPEED = 0.33;
static const MATH_PI = 3.141592653589793238;


struct point_t{
	float x;
	float y;
	float z;
	int travel_Time;
};

point_Zero = {x = 0.0, y = 0.0, z = 0.0, travel_Time = 0.0};

struct action_t{
	action_Type type;
	Robot target;
	point_t waypoints[10];
	float reward;
	float when_To_Act;
}

struct bounds_t{
	int x_Max;
	int y_Max;
}

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
    sim_CommandType_NoCommand = 0,   // continue doing whatever you are doing
    sim_CommandType_LandOnTopOf,     // trigger one 45 deg turn of robot (i)
    sim_CommandType_LandInFrontOf,   // trigger one 180 deg turn of robot (i)
    sim_CommandType_LandAtPoint,	 // land at a given point
    sim_CommandType_Track,           // follow robot (i) at a constant height
    sim_CommandType_Search           // ascend to 3 meters and go to (x, y)
};

struct observation_t
{
    float elapsed_time;
    float drone_x;
    float drone_y;
    bool  drone_cmd_done;

    float target_x[Num_Targets];
    float target_y[Num_Targets];
    float target_q[Num_Targets];

    float obstacle_x[Num_Obstacles];
    float obstacle_y[Num_Obstacles];
    float obstacle_q[Num_Obstacles];
};