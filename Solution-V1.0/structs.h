struct point_t{
	float x;
	float y;
	float z;
	int travel_Time;
};

struct action_t{
	action_Type type;
	Robot target;
	point_t waypoints[10];
	float reward;
	int when_To_Act;
}

struct bounds_t{
	int x_Max;
	int y_Max;
}

struct Plank
{
    point_t first;
    point_t last;
    float length;
	bool will_Exit_Green;
	bool will_Exit_Red;
};

enum ai_State_t
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

enum action_Type_t
{
    sim_CommandType_NoCommand = 0,   // continue doing whatever you are doing
    sim_CommandType_LandOnTopOf,     // trigger one 45 deg turn of robot (i)
    sim_CommandType_LandInFrontOf,   // trigger one 180 deg turn of robot (i)
    sim_CommandType_LandAtPoint,	 // land at a given point
    sim_CommandType_Track,           // follow robot (i) at a constant height
    sim_CommandType_Search           // ascend to 3 meters and go to (x, y)
};