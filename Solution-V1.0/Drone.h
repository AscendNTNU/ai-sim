	#include <cmath>
#include "structs.h"


class Drone{
private:
	point_t position;
	float orientation;
	float angle_Of_Motion;
	float speed;

	bool command_Done;

	point_t prev_Position;
	float prev_Orientation;

	drone_State_t drone_State;

public:

	//get
	point_t getPosition();
	drone_State_t getState();

	//update
	bool update(observation_t observation);


	//actions
	void wait(float time);
	int doAction(action_t action);
	// float getDistanceToPoint(point_t point);
	// float getTravelTimeToPoint(point_t point);
	point_t getIntersectionPoint(Robot robot);
}