#include <cmath>
#include "structs.h"


class Robot{
private:
	point_t position;
	float orientation;
	float robot_Timer;
	float speed;
	Plank curren_Plank;
public:
	point_t getPosition();
	float getOrientation();
	float getRobotTimer();
	float getSpeed();
	float getPlank();

	void update();
	
	bool isMoving();
}