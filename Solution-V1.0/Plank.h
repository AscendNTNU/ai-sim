#include "structs.h"

class Plank{
private:
    point_t endpoint_1; //First enpoint robot meets
    point_t endpoint_2; //Second endpoint robot meets
    float length;
    float reward;
    float angle;
    float grid_Value;
public:
	float getReward();
	float getAngle();
	float getLength();

	bool willExitGreen();
	bool willExitRed();
	float calculateReward();

};