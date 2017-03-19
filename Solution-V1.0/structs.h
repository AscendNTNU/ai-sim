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