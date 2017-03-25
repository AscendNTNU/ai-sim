#include "structs.h"
#include "AI.h"
#include "SimIn.h"

class World{
private:
	point_t origin;
	float time;
	float orientation;
	bounds_t bounds;
	AI ai;
	Input input;

public:

	//Constructors
	World();

	//Get
	point_t getOrigin();
	float getCurrentTime();
	float getOrientation();
	bounds_t getBounds();

	//Initialize
	bool startTimer();

	//Update
	bool update_world();
};