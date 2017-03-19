#include "structs.h"

class World{
private:
	point_t origin;
	float time;
	float orientation;
	bounds_t bounds;
public:
	point_t getOrigin();
	float getCurrentTime();
	float getOrientation();
	bounds_t getBounds();
};