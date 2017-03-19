#include Point
#include <tuple>

class World{
private:
	Point origin;
	float time;
	float orientation;
	int x_max;
	int y_max;
public:
	float getTime();
	float getOrientation();
	Bounds getBounds();
};