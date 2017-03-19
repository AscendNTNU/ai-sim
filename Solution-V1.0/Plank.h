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
	bool will_Exit_Green();
	bool will_Exit_Red();
};