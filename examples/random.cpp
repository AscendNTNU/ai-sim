#define SIM_IMPLEMENTATION
#define SIM_CLIENT_CODE
#include "../sim.h"
#include "../gui.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <string.h>
#include <math.h>
#include<iostream>
#include <algorithm>
#include <vector>
using namespace std;


int main()
{
    cout << "Random.cpp is now running!\n";
    sim_init_msgs(true);

    sim_Observed_State state;
    bool running = true;

    sim_Command cmd;

    int tick = 2;
    int k = 0;
    int abort_count = 30;
    while (running)
    {
        abort_count --;
        k++;
        sim_recv_state(&state);
        sim_Observed_State observed = state;
        if(observed.drone_cmd_done){
            tick--;
        }
        if((observed.drone_cmd_done && tick <= 0 )|| abort_count<= 0){

            int pick_random = rand() % 6;
            if(pick_random == 0){
                cmd.type = sim_CommandType_NoCommand;
            }else if(pick_random == 1){
                cmd.type = sim_CommandType_LandOnTopOf;
            }else if(pick_random == 2){
                cmd.type = sim_CommandType_LandInFrontOf;
            }else if(pick_random == 3){
                cmd.type = sim_CommandType_Track;
            }else if(pick_random == 4){
                cmd.type = sim_CommandType_Search;
            }else if(pick_random == 5){
                cmd.type = sim_CommandType_Land;
            }
            cmd.type = sim_CommandType_Land;
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
            cmd.i = rand() % Num_Targets;
            tick = 10;
            abort_count = 30;
            sim_send_cmd(&cmd);

        }
        /**for(int i = 0; i < Num_max_text_length*Num_Targets+Num_max_text_length; i++){
            cmd.text[i] = '$';
        }
        for(int i = 0; i < Num_max_text_length; i++){
            cmd.text[i] = 'A';
        }
        for (int i = 0; i < Num_Targets; i++){
             char str[] = "|det er ikke greit|";
             str[0] = '0'+ i;
             str[strlen(str)-1] = '0'+ i;
             int k = 0;
             str[strlen(str)] = '$';
             for (int bit = i*Num_max_text_length+Num_max_text_length; bit < i*Num_max_text_length+Num_max_text_length*2; bit++){
                 if(k>=strlen(str)){
                    break;
                 }
                 cmd.text[bit] = str[k];
                 k++;
             }


        }**/





        //Code for how to use custom debug text. Remember to include sim.h to use this.
        //All targets must have a text, and if you have less than 10 targets,
        //you have to delete some of the targets from the list below.
        //If you only want to debug a single groundrobot, all other texts may be left blank(as in "");
        //sim_Command cmd;
        const char *target_text[Num_Targets];
        target_text[0] = "Text for zeroth ground robot";
        target_text[1] = "Text for first ground robot";
        target_text[2] = "Text for second ground robot";
        target_text[3] = "Text for third ground robot";
        target_text[4] = "Text for fourth ground robot";
        target_text[5] = "Text for fifth ground robot";
        target_text[6] = "Text for sixt ground robot";
        target_text[7] = "Text for seventh ground robot";
        target_text[8] = "Text for eight ground robot";
        target_text[9] = "Text for ninth ground robot";

        //Set the command text
        get_char_text(cmd.text,"text for drone", target_text);

        //Send the text
        //sim_send_cmd(&cmd);

    }

    return 0;
}
