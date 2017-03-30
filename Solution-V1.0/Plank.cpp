#include "Plank.h"

<<<<<<< Updated upstream
Plank::Plank(point_t position, float angle, float time_Until_Turn, int num_Iterations){
	this.endpoint_1 = point_Zero;
	this.endpoint_2 = point_Zero;

    this.angle = angle;
	this.endpoint_1.x = time_Until_Turn*SPEED*cos(angle) + position.x;
    this.endpoint_1.y = time_Until_Turn*SPEED*sin(angle) + position.y;
    this.endpoint_2.x = (time_Until_Turn - 20+2.5)*SPEED*cos(angle) + position.x;
    this.endpoint_2.y = (time_Until_Turn - 20+2.5)*SPEED*sin(angle) + position.y;
=======
Plank::Plank(float x, float y, float angle, float time_Until_Turn, int num_Iterations){
	this->endpoint_1 = point_Zero;
	this->endpoint_2 = point_Zero;

    this->angle = angle;
	this->endpoint_1.x = time_Until_Turn*SPEED*cos(angle) + x;
    this->endpoint_1.y = time_Until_Turn*SPEED*sin(angle) + y;
    this->endpoint_2.x = (time_Until_Turn - 20+2.5)*SPEED*cos(angle) + x;
    this->endpoint_2.y = (time_Until_Turn - 20+2.5)*SPEED*sin(angle) + y;
>>>>>>> Stashed changes
    
    float dx = this->endpoint_2.x - this->endpoint_1.x;
    float dy = this->endpoint_2.y - this->endpoint_1.y;

    this->length = sqrt(dx*dx + dy*dy);

    this->reward = calculateReward(num_Iterations);
}

float Plank::getReward(){
	return this.reward;
}
float Plank::getAngle(){
	return this.angle;
}
float Plank::getLength(){
	return this.length;
}
bool Plank::will_Exit_Green(){
	return false; //Fix this
}
bool Plank::will_Exit_Red(){
	return false; //Fix this
}

float Plank::calculateReward(int n){
    float step_tot = this.length/n;
    float step_x = step_tot*cos(this.angle);
    float step_y = step_tot*sin(this.angle);

    float area = 0.0;  // signed area
    float x = 0.0;
    float y = 0.0;

    for (int i = 0; i < n; i++) {
        x = plank.x_1 + (i + 0.5) * step_x;
        y = plank.y_1 + (i + 0.5) * step_y;
        area += world.getGridValue(x, y) * step_tot; // sum up each small rectangle
    }
    // area = area/plank.length;
    return area;
}