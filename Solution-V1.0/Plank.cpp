#include "Plank.h"

Plank::Plank(float x, float y, float angle, float time_Until_Turn, int num_Iterations){
	this.endpoint_1 = point_Zero;
	this.endpoint_2 = point_Zero;

    this.angle = angle;
	this.endpoint_1.x = time_Until_Turn*SPEED*cos(angle) + x;
    this.endpoint_1.y = time_Until_Turn*SPEED*sin(angle) + y;
    this.endpoint_2.x = (time_Until_Turn - 20+2.5)*SPEED*cos(angle) + x;
    this.endpoint_2.y = (time_Until_Turn - 20+2.5)*SPEED*sin(angle) + y;
    
    float dx = this.endpoint_2.x - this.endpoint_1.x;
    float dy = this.endpoint_2.y - this.endpoint_1.y;

    this.length = sqrt(dx*dx + dy*dy);

    this.reward = calculateReward(num_Iterations);
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

float Plank::gridValue(float X, float Y){
    float value = (-9.995004e+02)+(9.976812e+01)*X+(-1.004701e+02)*Y
        +(-5.785388e+01)*pow(X,2)+(1.161562e+01)*X*Y+(5.477725e+01)*pow(Y,2)
        +(1.260229e+01)*pow(X,3)+(1.299816e+01)*pow(X,2)*Y+(-1.438667e+01)*X*pow(Y,2)+(-1.158062e+01)*pow(Y,3)
        +(-1.404096e+00)*pow(X,4)+(-3.106303e+00)*pow(X,3)*Y+(4.263504e-01)*pow(X,2)*pow(Y,2)
        +(2.851553e+00)*X*pow(Y,3)+(1.301842e+00)*pow(Y,4)
        +(9.053408e-02)*pow(X,5)+(2.901147e-01)*pow(X,4)*Y+(1.327346e-01)*pow(X,3)*pow(Y,2)
        +(-1.761180e-01)*pow(X,2)*pow(Y,3)+(-2.603853e-01)*X*pow(Y,4)+(-8.415694e-02)*pow(Y,5)
        +(-3.615309e-03)*pow(X,6)+(-1.235169e-02)*pow(X,5)*Y+(-1.602868e-02)*pow(X,4)*pow(Y,2)
        +(3.840976e-03)*pow(X,3)*pow(Y,3)+(1.239923e-02)*pow(X,2)*pow(Y,4)
        +(1.283802e-02)*X*pow(Y,5)+(3.201336e-03)*pow(Y,6)
        +(8.890888e-05)*pow(X,7)+(1.960570e-04)*pow(X,6)*Y+(7.353331e-04)*pow(X,5)*pow(Y,2)
        +(-9.145182e-05)*pow(X,4)*pow(Y,3)+(8.794847e-10)*pow(X,3)*pow(Y,4)
        +(-6.113303e-04)*pow(X,2)*pow(Y,5)+(-2.451141e-04)*X*pow(Y,6)+(-7.627948e-05)*pow(Y,7)
        +(-1.058445e-06)*pow(X,8)+(4.059809e-11)*pow(X,7)*Y+(-1.167195e-05)*pow(X,6)*pow(Y,2)
        +(-4.630460e-12)*pow(X,5)*pow(Y,3)+(-1.355465e-11)*pow(X,4)*pow(Y,4)
        +(-5.731993e-12)*pow(X,3)*pow(Y,5)+(1.167198e-05)*pow(X,2)*pow(Y,6)
        +(3.539047e-11)*X*pow(Y,7)+(1.058675e-06)*pow(Y,8);

    return value;
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
        area += gridValue(x, y) * step_tot; // sum up each small rectangle
    }
    // area = area/plank.length;
    return area;
}