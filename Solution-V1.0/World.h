#include "structs.h"
#include "AI.h"
#include "Sim.h"

class World{
private:
	point_t origin;
	float time;
	float orientation;
	bounds_t bounds;
	AI ai;
	Input input;

public:
	World();
	point_t getOrigin();
	float getCurrentTime();
	float getOrientation();
	bounds_t getBounds();
	bool startTimer();
};