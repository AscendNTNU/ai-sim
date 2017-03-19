#include <cmath>
#include "structs.h"

class Drone{
private:
	point_t position;
	float orientation;

	float angle_Of_Motion;
	float speed;

	drone_State_t drone_State;

public:
	point_t getPosition();
	drone_State_t getState();

	void wait(float time);

	int doAction(action_t action);

	// float getDistanceToPoint(point_t point);
	// float getTravelTimeToPoint(point_t point);
	point_t getIntersectionPoint(Robot robot);

}