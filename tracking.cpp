#define SIM_IMPLEMENTATION
#define SIM_CLIENT_CODE
#include "sim.h"
#include "gui.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <string.h>
#include <math.h>
#include<iostream>
#include <algorithm>
#include <vector>
using namespace std;

class Filter{
    public:
        static void box_blur(float (*src)[2*20*pixels_each_meter][2*20*pixels_each_meter],float (*trg)[2*20*pixels_each_meter][2*20*pixels_each_meter],float r,float degenerate){
            int width = 2*20*pixels_each_meter;
            float divide = 1.0/(r + r + 1.0);
            float exp = degenerate;
            for(int i = 0; i<width;i++){
                for(int j = 1; j<width-1;j++){
                    (*trg)[i][j] = exp*divide*((*src)[i][j-int(ceil(r))]*r + (*src)[i][j] + (*src)[i][j+int(ceil(r))]*r);
                }
            }
            for(int j = 0; j<width;j++){
                for(int i = 1; i<width-1;i++){
                    (*src)[i][j] = exp*divide*((*trg)[i-int(ceil(r))][j]*r + (*trg)[i][j] + (*trg)[i+int(ceil(r))][j]*r);
                }
            }
            for(int j = 0; j<width;j++){
                (*src)[0][j] = (*src)[1][j];
                (*src)[width-1][j] = (*src)[width-2][j];
            }
            for(int i = 0; i<width;i++){
                (*src)[i][0] = (*src)[i][1];
                (*src)[i][width-1] = (*src)[i][width-2];
            }

        }


};


float uniform_shift(float x,float x_min){
    if( x >= x_min){
        return 1.0;
    }else{
        return 0.0;
    }
}

float gaussian_disp_2d(float x,float y, float std_x,float std_y,float mean_x,float mean_y){
    return exp(-((pow((x-mean_x),2))/(2*pow(std_x,2)) + (pow((y-mean_y),2))/(2*pow(std_y,2))));
}

float  normal_disp_2d(float x,float y, float std_x,float std_y,float mean_x,float mean_y){
    float p = 0;
    float weight = 1.0/(2.0*M_PI*std_x*std_y*(sqrt(1-pow(p,2))));
    float potence_weight = -1.0/(2*(1-pow(p,2)));
    float remove = 2.0*p*(x-mean_x)*(y-mean_y)/(std_x*std_y);
    float term_x = (pow((x-mean_x),2))/(pow(std_x,2));
    float term_y = (pow((y-mean_y),2))/(pow(std_y,2));
    return weight*exp(potence_weight*(term_x+term_y-remove));
}

float transformed_normal_disp(float x,float y, float std_x,float std_y, float mean_x,float mean_y, float angle){
    float translated_x = x - mean_x;
    float translated_y = y - mean_y;

    float new_x = translated_x*cos(-angle) - translated_y*sin(-angle);
    float new_y = translated_x*sin(-angle) + translated_y*cos(-angle);
    return normal_disp_2d(new_x,new_y, std_x,std_y,0,0);
}

float normal_disp(float x,float v,float mean){
    return 1.0/(sqrt(2*M_PI*pow(v,2)))*exp(-(pow((x-mean),2))/(2*pow(v,2)));
}

float sigmoid(float x,float thermal,float center){
    if(thermal == 0.0){
        return uniform_shift(x,center);
    }
    try{
        return 1.0 / (1.0 + exp(-(1.0 / thermal ) * (x - center)));
    } catch (int e){
        ;
    }
}


class PaintCanvas{
    public:
        float image[2*20*pixels_each_meter][2*20*pixels_each_meter];
    public:
        PaintCanvas(){
            for(int i =0; i<2*20*pixels_each_meter; i++){
                for(int j = 0; j<2*20*pixels_each_meter; j++){
                    image[i][j] = 0.0;
                }
            }
        }
        ~PaintCanvas(){
        }

        float getValue(float x_center,float y_center,float offset_x,float offset_y ){

            offset_y = (offset_y + 20);
            offset_x = (offset_x + 20);
            y_center = 20.0-y_center;
            offset_y = 20.0-offset_y;
            float x = (x_center+offset_x)*pixels_each_meter;
            float y = (y_center+offset_y)*pixels_each_meter;
            int value_x = int(round(x));
            int value_y = int(round(y));
            if(value_x>=2*20*pixels_each_meter)
                return -1;
            if(value_y>=2*20*pixels_each_meter)
                return -1;
            if(value_x<0)
                return -1;
            if(value_y<0)
                return -1;

            float h = image[value_y][value_x];
            float slide;
            float up;
            /*
            if(value_x > 20*pixels_each_meter){
                slide = 0.0;
            }else{
                slide = 0.1;
            }
            if(value_y> 20*pixels_each_meter){
                up = 0.2;
            }else{
                up = 0.3;
            }*/

            //debug

            return h;// + up + slide;
        }
        void tick(){
            float trg[2*20*pixels_each_meter][2*20*pixels_each_meter];
            Filter::box_blur(&image,&trg,0.5,0.99999);
            //memcpy(&image, &trg, sizeof(image));
        }
        void paint_at(float drone_x,float drone_y,float bot_x,float bot_y ,float r){
            float offset_x = drone_x - bot_x;
            float offset_y = drone_y - bot_y;
            r = r*pixels_each_meter;
            float x = 20*pixels_each_meter+offset_x*pixels_each_meter;
            float y = 20*pixels_each_meter-offset_y*pixels_each_meter;
            int value_x = int(round(x));
            int value_y = int(round(y));

            for(int i = value_y-int(floor(r)); i<value_y+int(ceil(r)); i++){
                if(i < 0){
                    continue;
                }
                if(i >= 2*20*pixels_each_meter){
                    continue;
                }

                for(int j = value_x-int(floor(r)); j<value_x+int(ceil(r)); j++){

                    if(j < 0){
                        continue;
                    }
                    if(j >= 2*20*pixels_each_meter){
                        continue;
                    }

                    float cathesus_x = (float(i)- value_y);
                    float cathesus_y = (float(j)- value_x);
                    float l = sqrt(pow(cathesus_x,2.0)+pow(cathesus_y,2.0));
                    if( l < r){
                        image[i][j] = 1.0;
                    }
                }
            }


        }
        void paint_outside_borders(float offset_x,float offset_y){
            offset_x = -offset_x+30;
            offset_y = -offset_y+10;
            offset_y = 20-offset_y;
            offset_x = offset_x*pixels_each_meter;
            offset_y = offset_y*pixels_each_meter;

            paint_outside_bounding_box(offset_x-10*pixels_each_meter,offset_x+10*pixels_each_meter,offset_y-10*pixels_each_meter,offset_y+10*pixels_each_meter);


        }
        void paint_outside_bounding_box(float x_min,float x_max,float y_min,float y_max){
            int x_max_int = int(ceil(x_max));
            int x_min_int = int(floor(x_min));
            int y_max_int = int(ceil(y_max));
            int y_min_int = int(floor(y_min));

            for(int i = 0; i<20*2*pixels_each_meter; i++){

                for(int j = 0; j<20*2*pixels_each_meter; j++){


                    if(!(j >= x_min_int && j <= x_max_int && i >= y_min_int && i <= y_max_int)){
                        image[i][j] = 1.0;
                    }
                }
            }


        }
};
class GroundRobot{
    public:
        int id;
        float q;
        float plank_angle;
        float observed_at_time;
        bool is_reversing;
        bool save_plank_angle;
        bool depleting;
        bool has_removed_model;
        float x;
        float y;


};

struct PosPair{
    float x;
    float y;
};

class GroundRobotTrackModel{
    #include <vector>
    public:
        int *id;
        float *init_timecode;
        float *init_angle;
        float *init_pos_x;
        float *init_pos_y;
        float *init_turn_timecode;
        PaintCanvas *paint_canvas;
        float *volume;
        float *volume_summer;
        PosPair *top_point_pos;
        float *top_point_value;
        GroundRobotTrackModel(int idq,float init_timecodeq,float init_angleq,float init_pos_xq,float init_pos_yq,float init_turn_timecodeq){

            id = new int;
            init_timecode = new float;
            init_angle = new float;
            init_pos_x = new float;
            init_pos_y = new float;
            init_turn_timecode = new float;
            paint_canvas = new PaintCanvas();
            volume = new float;
            volume_summer = new float;
            top_point_pos = new PosPair();
            top_point_value = new float;
            *id = idq;
            *init_timecode = init_timecodeq;
            *init_angle = init_angleq;
            *init_pos_x = init_pos_xq;
            *init_pos_y = init_pos_yq;
            *init_turn_timecode = init_turn_timecodeq;
            *top_point_value = 0;
            (*top_point_pos).x = 0;
            (*top_point_pos).y = 0;


            *volume = 1.0;
            *volume_summer = 0.0;
        }
        ~GroundRobotTrackModel(){
            delete id,init_timecode,init_angle,init_pos_x,init_pos_y,init_turn_timecode,paint_canvas;
        }

        float turn_function(float x){
            if(x<2.5){
                return 0;
            }else if(x>=2.5 && x < 20){
                return 0.057142857143*(x-2.5);
            }else if(x>=20 && x<22.5){
                return 1.0;
            }else if(x>=22.5){
                return 1-0.057142857143*(x-22.5);
            }

        }

        float get_value(float timecode,float x,float y){
            //float std_x = 0.1;
            //float std_y = 0.1;
            float std_x = (timecode - *init_timecode+0.1)/100.0;
            float std_y = (timecode - *init_timecode+0.1)/40.0;
            PosPair mean_pair = get_mean_pair(timecode);
            float mean_x = mean_pair.x;
            float mean_y = mean_pair.y;
            float height_disp = transformed_normal_disp(x,y,std_x,std_y,mean_x,mean_y,*init_angle);
            float observation_value = (*paint_canvas).getValue(x,y,-mean_x,-mean_y); //get_observation_value(observations,timecode,x,y);
            return (1-observation_value)*1*height_disp;
            //return (1-observation_value ) * 1* height_disp * (1.0 / ( float(pixels_each_meter ) * float(pixels_each_meter)));

            return observation_value;

        }
        void paint_at(float x,float y,float r,float timecode){

            PosPair mean_pair = get_mean_pair(timecode);
            float mean_x = mean_pair.x;
            float mean_y = mean_pair.y;
            (*paint_canvas).paint_at(x,y,mean_x,mean_y,r);

        }
        void paint_outside_borders(float timecode){
            PosPair mean_pair = get_mean_pair(timecode);
            float mean_x = mean_pair.x;
            float mean_y = mean_pair.y;
            (*paint_canvas).paint_outside_borders(mean_x,mean_y);
        }



        PosPair get_mean_pair(float timecode){

            //TODO
            //DECREASE HOW LONG FROM THE CENTER MEAN CAN GO OVER TIME

            //float std_x = 0.3;
            //float std_y = 0.3;
            //float est_time_since_turn = fmod((timecode  - this->init_turn_timecode), 20.0);
            //float est_plank_length_behind = est_time_since_turn * Robot_Speed;
            float original_plank_length_behind = (fmod(*init_timecode - *init_turn_timecode,20.0) -2.5 )* Robot_Speed;
            //float est_plank_length_ahead = (20.0 - est_time_since_turn) * Robot_Speed;
            float period_duration = 40.0;
            float est_moment_in_period = fmod((timecode  - *init_turn_timecode),period_duration);
            float plank_pos_percent = turn_function(est_moment_in_period);
            float plank_start_x =  *init_pos_x - original_plank_length_behind*cos(*init_angle);
            float plank_start_y =  *init_pos_y - original_plank_length_behind*sin(*init_angle);
            float mean_x = plank_start_x + 17.5*Robot_Speed*cos(*init_angle)*plank_pos_percent;
            float mean_y = plank_start_y + 17.5*Robot_Speed*sin(*init_angle)*plank_pos_percent;
            PosPair pair;
            pair.x = mean_x;
            pair.y = mean_y;
            return pair;
        }


        /**
        float get_observation_value(std::vector<Observation*> observations,float timecode,float x,float y){

            float product = 1.0;
            for (int i = 0; i < observations.size() ; i++){
                PosPair mean_pair = get_mean_pair((*observations[i]).init_timecode);
                PosPair mean_pair_now = get_mean_pair(timecode);
                float init_offset_x = -mean_pair_now.x +  mean_pair.x;
                float init_offset_y = -mean_pair_now.y +  mean_pair.y;
                product *= (*observations[i]).get_value(timecode,x+init_offset_x,y+init_offset_y);
            }
            return product;
        }**/

};

int main()
{
    cout << "Tracking.cpp is now running!\n";
    sim_init_msgs(true);
    srand(time(NULL));
    sim_State state;
    bool running = true;
    vector<GroundRobotTrackModel*> tracking_models;
    vector<GroundRobot> bots_in_view;
    int grb_id_counter = 1;
    sim_Command cmd;
    //vector<Observation*> observations;

    //m is the chance that the programmed hasnt removed some models due to a mistake
    //when m gets small there is a high chance of there being uniformed untraced models.
    //m can be used as a guide for keeping track of the world state
    float m = 1.0;

    //the chance of finding 1 drone somewhere at any location on the map;
    float uniform = 0.00196349;
    float removed_models_count = 0;

    int k = 0;
    while (running)
    {
        k++;
        sim_recv_state(&state);
        sim_Observed_State observed = sim_observe_state(state);
        float timecode_now = observed.elapsed_time;

        for (int r_id = 0; r_id < sizeof(observed.target_in_view); r_id++){
            if( observed.target_in_view[r_id]){


                //find the correct bot and update it
                bool found = false;
                for (int i = 0; i < bots_in_view.size() ; i++){
                    if(bots_in_view[i].id == r_id){
                        found = true;
                        bots_in_view[i].q = observed.target_q[r_id];
                        bots_in_view[i].is_reversing = observed.target_reversing[r_id];
                        bots_in_view[i].x = observed.target_x[r_id];
                        bots_in_view[i].y = observed.target_y[r_id];
                        bots_in_view[i].observed_at_time = timecode_now;


                        //logic for toggling plank angle
                        cout << "i: " << "plank_angle before"<< bots_in_view[i].plank_angle <<endl;
                        cout << "i:" << " is_reversing" << bots_in_view[i].is_reversing << endl;
                        cout << "i: " << "save plank angle" << bots_in_view[i].save_plank_angle <<endl;

                        if(bots_in_view[i].is_reversing){
                            if( bots_in_view[i].save_plank_angle){
                                bots_in_view[i].plank_angle = bots_in_view[i].plank_angle-PI;
                            }

                            bots_in_view[i].save_plank_angle = false;



                        }else{
                            bots_in_view[i].plank_angle = observed.target_q[r_id];
                            bots_in_view[i].save_plank_angle = true;

                        }
                        cout << "i: " << "plank_angle after"<< bots_in_view[i].plank_angle <<endl;
                        break;
                    }
                }
                //the bot i a new bot so init it and put it into list
                if(found == false){
                    GroundRobot robot;
                    robot.id = r_id;
                    robot.q = observed.target_q[r_id];
                    robot.is_reversing =observed.target_reversing[r_id];
                    robot.x =observed.target_x[r_id];
                    robot.y =observed.target_y[r_id];
                    robot.depleting = false;
                    robot.save_plank_angle = true;
                    robot.has_removed_model = false;
                    robot.observed_at_time = timecode_now;

                    //TODO:
                    robot.plank_angle = robot.q; //bad way of doing it
                    //what happens if the bot is turning when you start see it.
                    //Please estimate the plank angle
                    bots_in_view.push_back(robot);
                    cout << "bot in line of sight! (id: " << r_id << ") \n";
                    //cout << "state: bots_in_view: " << bots_in_view.size() << "\t tracking: " << tracking_models.size() << "\n";


                }
            }else{
                for (int i = 0; i < bots_in_view.size() ; i++){
                     if(bots_in_view[i].id == r_id){
                         bots_in_view[i].depleting = true;
                         break;
                     }
                }
            }
        }


        for (int item = 0; item < bots_in_view.size(); item++){
            if( bots_in_view[item].depleting == false){
                //a bot has entered view. Remove it from the tracked models.
                //if (bots_in_view[item].has_removed_model == false){
                    //Okei it is processed. It must not affect change anylonger
                    bots_in_view[item].has_removed_model = true;
                    if(tracking_models.size()>0){
                        float prob_lead_trcmodel_is_bot = 0;
                        int lead_trcmodel_id = *tracking_models.at(0)->id;
                        float prob_second_trcmodel_is_bot = 0;
                        int second_trcmodel_id = -1;
                        float not_some_other_model = 1.0;
                        for (int i = 0; i < tracking_models.size() ; i++){

                            float cand_prob = tracking_models.at(i)->get_value(timecode_now,bots_in_view[item].x,bots_in_view[item].y);
                            not_some_other_model *= (1.0-cand_prob);
                            if(cand_prob > prob_lead_trcmodel_is_bot){
                                prob_second_trcmodel_is_bot     = prob_lead_trcmodel_is_bot;
                                second_trcmodel_id              = lead_trcmodel_id;
                                prob_lead_trcmodel_is_bot       = cand_prob;
                                lead_trcmodel_id                = (*tracking_models.at(i)->id);
                            }
                        }
                        //calculate the prob that there is some model m that is not being tracked;
                        //(TODO: Improve it by collecting all old models which is removed)
                        m = m;

                        //calculate prob that the given model is not some other model
                        //(TODO: See into having count of models affect m)
                        //not_some_other_model = not_some_other_model / (1.0-prob_lead_trcmodel_is_bot);
                        //cout << "not_some_other_model:" << not_some_other_model << "\n";
                        //cout << "m:" << m << "\n";
                        //float prob_not_finding_uniform_at_pos = pow((1.0-uniform),removed_models_count);

                        //There is some chance there has been a mistake. In that case use the uniform disp
                        //not_some_other_model = not_some_other_model * 1.0 + not_some_other_model * (1.0-m)*prob_not_finding_uniform_at_pos ;
                        cout << "not_some_other_model and not m:" << not_some_other_model << "\n";

                        //if its not the other model,then remove it
                        if(timecode_now > 100){//0.95){


                               //update m;
                               //m *= not_some_other_model;

                               //increase the amount of models removed
                               removed_models_count ++;

                               //search through and remove

                               for (int i = 0; i < tracking_models.size() ; i++){
                                       if(*(tracking_models[i]->id) == lead_trcmodel_id){

                                            cout << "Removing tracking model. (Tag: " << *(tracking_models[i]->id) << "). Guilty id: " << bots_in_view[item].id <<" \n";
                                            delete tracking_models[i];
                                            tracking_models.erase(tracking_models.begin() + i );
                                            break;
                                       }
                               }

                         }
                       // }
                    }
            }else{
                //Make tracking models
                if(tracking_models.size() < 3){
                    grb_id_counter++;
                    cout << "plank_angle at create:" << bots_in_view[item].plank_angle << endl;
                    cout << "time at create" << timecode_now <<endl;
                    cout << "time set to bot" << bots_in_view[item].observed_at_time << endl;
                    cout << "bots_in_view[item].observed_at_time - fmod(bots_in_view[item].observed_at_time,20.0)" << bots_in_view[item].observed_at_time- fmod(bots_in_view[item].observed_at_time,20.0) << endl;
                    GroundRobotTrackModel *track_this = new GroundRobotTrackModel(grb_id_counter,bots_in_view[item].observed_at_time,bots_in_view[item].plank_angle,bots_in_view[item].x,bots_in_view[item].y,bots_in_view[item].observed_at_time - fmod(bots_in_view[item].observed_at_time,20.0));
                    tracking_models.push_back(track_this);
                    //remove depleting bot
                    bots_in_view.erase(bots_in_view.begin() + item);
                    //cout << "bot (id: " << item << ") out of sight!" << "\n";
                    //cout << "state: bots_in_view: " << bots_in_view.size() << "\t tracking: " << tracking_models.size() << "\n";
                    cout << "Creating tracking model. (Tag: " << *(track_this->id) << ")\n";
                }



            }

        }

        //TODO:
        //Remove tracking models after time (TODO: Remove them after entropy)
        if(tracking_models.size() > Num_Targets){
            for (int i = 0; i < tracking_models.size() ; i++){
                if(timecode_now - *(tracking_models.at(i)->init_timecode) > 80){
                    //cout << "Removing tracking model due to entropy. (Tag: " << current_grbot.id << ")\n";
                    delete tracking_models.at(i);
                    tracking_models.erase(tracking_models.begin() + i );
                    break;
                }



            }
        }

        /**
        //add observations:
        Observation observation;
        observation.init_center_x = state.drone.x;
        observation.init_center_y = state.drone.y;
        observation.radius = Sim_Drone_View_Radius;
        observation.init_timecode = timecode_now;

        observations.push_back(&observation);     **/



        //blur portrait
        for (int i = 0; i < tracking_models.size() ; i++){
            (*tracking_models.at(i)->paint_canvas).tick();
        }

        //paint portrait
        bool found_one = false;
        for (int r_id = 0; r_id < sizeof(observed.target_in_view); r_id++){
            if( observed.target_in_view[r_id]){
                found_one = true;
                break;
            }
        }

        if(found_one == false){
            for (int i = 0; i < tracking_models.size() ; i++){

                tracking_models.at(i)->paint_at(observed.drone_x,observed.drone_y,compute_drone_view_radius(observed.drone_z),timecode_now);
                tracking_models.at(i)->paint_outside_borders(timecode_now);
            }
        }


        //get position of heighest value
        float mountain_top = 0.0;
        PosPair go_to;


        go_to.x = rand() % 19;
        go_to.y = rand() % 19;


        // make toppoints to zero
        for (int i = 0; i < tracking_models.size() ; i++){
                (*tracking_models.at(i)->top_point_value) = 0;

        }

        //plot data
        float step_size = 1.0 / pixels_each_meter;
        for (int x_bit = 0; x_bit < pixels_each_meter*20 ; x_bit++)
        {
            for (int y_bit = 0; y_bit < pixels_each_meter*20 ; y_bit++){

                float x_pos = float(x_bit) * step_size;
                float y_pos = float(y_bit) * step_size;


                float sum = 0;
                for (int i = 0; i < tracking_models.size() ; i++){
                    float current_bot_pixel_value = tracking_models.at(i)->get_value(timecode_now,x_pos,y_pos);
                    sum += current_bot_pixel_value/(*tracking_models.at(i)->volume);

                    //find top point
                    if(current_bot_pixel_value >= *tracking_models.at(i)->top_point_value){
                        (*tracking_models.at(i)->top_point_value) = current_bot_pixel_value;
                        (*tracking_models.at(i)->top_point_pos).x = x_pos;
                        (*tracking_models.at(i)->top_point_pos).y = y_pos;
                    }


                    //increase the volume of this bot after observation
                    *tracking_models.at(i)->volume_summer+=current_bot_pixel_value;
                }

                cmd.heatmap[y_bit * pixels_each_meter*20 + x_bit] = sum;

                //greedy algorithm
                float length = sqrt(pow(observed.drone_x-x_pos,2.0)+pow(observed.drone_y-y_pos,2.0));
                if((20.0-length)/25.0*sum> mountain_top){
                    mountain_top = sum;
                    if(timecode_now > 100){
                        go_to.x = x_pos;
                        go_to.y = y_pos;
                    }
                }



            }
        }
        //update volume
        for (int i = 0; i < tracking_models.size() ; i++){
            *tracking_models.at(i)->volume = *(tracking_models.at(i)->volume_summer);
            *tracking_models.at(i)->volume_summer = 0.0;
        }

        //draw toppoints
        for (int i = 0; i < tracking_models.size() ; i++){

            int x_bit = int(round(    (*tracking_models.at(i)->top_point_pos).x/step_size    ));
            int y_bit = int(round(    (*tracking_models.at(i)->top_point_pos).y/step_size     ));
            cmd.heatmap[y_bit * pixels_each_meter*20 + x_bit] = -1000;

        }


        if(observed.drone_cmd_done){
            /**
            cmd.type = sim_CommandType_Search;
            cmd.x = observed.drone_x + rand() % 20 -10;
            cmd.y = observed.drone_y + rand() % 20 -10;
            if(cmd.x>20.0)
                cmd.x = 10.0;
            if(cmd.y>20.0)
                cmd.y = 10.0;
            if(cmd.x<0.0)
                cmd.x = 10.0;
            if(cmd.y<0.0)
                cmd.y = 10.0;
            **/
            cmd.type = sim_CommandType_Search;
            cmd.x = go_to.x;
            cmd.y = go_to.y;

        }
        cmd.i = 0;
        sim_send_cmd(&cmd);



        
    }

    return 0;
}
