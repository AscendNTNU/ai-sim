#define SIM_IMPLEMENTATION
#include "sim.h"
#include "gui.h"
#include <algorithm>
#include <vector>
#include "lib/jo_gif.cpp"
#include <time.h>
#include <stdlib.h>

#include <stdint.h>
typedef float       r32;
typedef uint64_t    u64;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint8_t     u08;
typedef int32_t     s32;
typedef int16_t     s16;
typedef int8_t      s08;
#define global      static
#define persist     static

#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_assert.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "lib/imgui/imgui_draw.cpp"
#include "lib/imgui/imgui.cpp"
#include "lib/imgui/imgui_demo.cpp"
#include "lib/imgui/imgui_impl_sdl.cpp"

// Allocate thirty minutes worth of real time history
#define History_Max_Length ((int)(10.0f * 60.0f / Sim_Timestep))

#define Assert SDL_assert
#define Printf SDL_Log
static int fps_lock = 15;

struct Color
{
    r32 r, g, b, a;
};

struct VideoMode
{
    SDL_Window *window;

    int width;
    int height;
    int gl_major;
    int gl_minor;
    int double_buffer;
    int depth_bits;
    int stencil_bits;
    int multisamples;

    // 0 for immediate updates, 1 for updates synchronized with the
    // vertical retrace. If the system supports it, you may
    // specify -1 to allow late swaps to happen immediately
    // instead of waiting for the next retrace.
    int swap_interval;

    // Instead of using vsync, you can specify a desired framerate
    // that the application will attempt to keep. If a frame rendered
    // too fast, it will sleep the remaining time. Leave swap_interval
    // at 0 when using this.
    int fps_lock;
};

struct Noise_Object{
  float position_offset_target;
  float freq_position_offset_target;

  float position_offset_target_horizon;
  float freq_noise_horizon;

  float percent_target_in_view;

  float angle_offset_target;
  float freq_angle_offset_target;

  float angle_offset_down_camera;
  float freq_angle_offset_down_camera;

  float position_offset_drone;
  float freq_position_offset_drone;
};

const char *gl_error_message(GLenum error)
{
    switch (error)
    {
    case 0: return "NO_ERROR";
    case 0x0500: return "INVALID_ENUM";
    case 0x0501: return "INVALID_VALUE";
    case 0x0502: return "INVALID_OPERATION";
    case 0x0503: return "STACK_OVERFLOW";
    case 0x0504: return "STACK_UNDERFLOW";
    case 0x0505: return "OUT_OF_MEMORY";
    case 0x0506: return "INVALID_FRAMEBUFFER_OPERATION";
    default: return "UNKNOWN";
    }
}

u64 get_tick()
{
    return SDL_GetPerformanceCounter();
}

r32 get_elapsed_time(u64 begin, u64 end)
{
    u64 frequency = SDL_GetPerformanceFrequency();
    return (r32)(end - begin) / (r32)frequency;
}

r32 time_since(u64 then)
{
    u64 now = get_tick();
    return get_elapsed_time(then, now);
}

global r32 NDC_SCALE_X;
global r32 NDC_SCALE_Y;

global sim_State STATE;
global sim_State HISTORY_STATE[History_Max_Length];
global sim_Observed_State HISTORY_OBSERVED_STATE[History_Max_Length];
global sim_Command HISTORY_CMD[History_Max_Length];
global int HISTORY_LENGTH;

sim_Observed_State generate_noise(sim_State state, Noise_Object noise )
{
    double offset_x;
    double offset_y;
    double offset_z;
    double offset_angle;
    double freq;
    sim_Observed_State result = {};
    result.elapsed_time = state.elapsed_time;
    //generate noise for drone

    //generate x offset

    freq = (float)rand()/(float)(RAND_MAX/noise.freq_position_offset_drone);
    if(freq > 1.0){
      offset_x = (float)rand()/(float)(RAND_MAX/noise.position_offset_drone)-noise.position_offset_drone/2.0;
    }else{
      offset_x = 0.0;
    }
    //generate y offset
    freq = (float)rand()/(float)(RAND_MAX/noise.freq_position_offset_drone);
    if(freq > 1.0){
      offset_y = (float)rand()/(float)(RAND_MAX/noise.position_offset_drone)-noise.position_offset_drone/2.0;
    }else{
      offset_y = 0.0;
    }

    //generate z offset
    freq = (float)rand()/(float)(RAND_MAX/noise.freq_position_offset_drone);
    if(freq > 1.0){
      offset_z = (float)rand()/(float)(RAND_MAX/noise.position_offset_drone)-noise.position_offset_drone/2.0;
    }else{
      offset_z = 0.0;
    }

    //add noise
    result.drone_x = state.drone.x + offset_x;
    result.drone_y = state.drone.y + offset_y;
    result.drone_z = state.drone.z + offset_z;

    result.drone_cmd_done = state.drone.cmd_done;

    sim_Robot *targets = state.robots;
    sim_Robot *obstacles = state.robots + Num_Targets;
    float visible_radius_down_camera = compute_drone_view_radius(state.drone.z);


    for (unsigned int i = 0; i < Num_Targets; i++)
    {
        float dx = state.drone.x - targets[i].x;
        float dy = state.drone.y - targets[i].y;

        float distance = vector_length(dx,dy);
        float unit_x = dx/distance;
        float unit_y = dy/distance;
        float sin_angle =  distance / vector_length(distance,state.drone.z);
        float cos_angle =  state.drone.z / vector_length(distance,state.drone.z);
        freq = (float)rand()/(float)(RAND_MAX/1.0);
        if (freq*sin_angle >= noise.percent_target_in_view ){
            result.target_in_view[i] = false;
        }else{
            result.target_in_view[i] = true;
        }
        result.target_removed[i] = targets[i].removed;
        result.target_reward[i] = targets[i].reward;

        freq = (float)rand()/(float)(RAND_MAX/noise.freq_position_offset_target);
        if(freq > 1.0){

          offset_x = (float)rand()/(float)(RAND_MAX/noise.position_offset_target)-noise.position_offset_target/2.0;
        }else{
          offset_x = 0.0;
        }
        //generate y offset
        freq = (float)rand()/(float)(RAND_MAX/noise.freq_position_offset_target);
        if(freq > 1.0){

          offset_y = (float)rand()/(float)(RAND_MAX/noise.position_offset_target)-noise.position_offset_target/2.0;
        }else{
          offset_y = 0.0;
        }

        freq = (float)rand()/(float)(RAND_MAX/noise.freq_noise_horizon);
        float horizon_x = 0;
        float horizon_y = 0;
        if(freq > 1.0){
          float t = (float)rand()/(float)(RAND_MAX/noise.position_offset_target_horizon) - noise.position_offset_target_horizon/2.0;
          horizon_x = t*(1.0-cos_angle)*unit_x;
          horizon_y = t*(1.0-cos_angle)*unit_y;
        }


        result.target_x[i] = targets[i].x + offset_x+horizon_x;
        result.target_y[i] = targets[i].y + offset_y+horizon_y;


        if(distance < visible_radius_down_camera){
          freq = (float)rand()/(float)(RAND_MAX/noise.freq_angle_offset_down_camera);
          if(freq > 1.0){
            offset_angle = (float)rand()/(float)(RAND_MAX/(noise.angle_offset_down_camera*0.0174533))-(noise.angle_offset_down_camera*0.0174533)/2.0;
          }else{
            offset_angle = 0.0;
          }
        }else{
          freq = (float)rand()/(float)(RAND_MAX/noise.freq_angle_offset_target);
          if(freq > 1.0){
            offset_angle = (float)rand()/(float)(RAND_MAX/(noise.angle_offset_target*0.0174533))-(noise.angle_offset_target*0.0174533)/2.0;
          }else{
            offset_angle = 0.0;
          }

        }
        result.target_q[i] = targets[i].q + offset_angle;
        result.target_reversing[i] = (targets[i].state == Robot_Reverse);

    }
    for (unsigned int i = 0; i < Num_Obstacles; i++)
    {
        float dx = state.drone.x - obstacles[i].x;
        float dy = state.drone.y - obstacles[i].y;

        float distance = vector_length(dx,dy);
        float unit_x = dx/distance;
        float unit_y = dy/distance;
        float sin_angle =  distance / vector_length(distance,state.drone.z);
        float cos_angle =  state.drone.z / vector_length(distance,state.drone.z);
        freq = (float)rand()/(float)(RAND_MAX/1.0);


        freq = (float)rand()/(float)(RAND_MAX/noise.freq_position_offset_target);
        if(freq > 1.0){

          offset_x = (float)rand()/(float)(RAND_MAX/noise.position_offset_target)-noise.position_offset_target/2.0;
        }else{
          offset_x = 0.0;
        }
        //generate y offset
        freq = (float)rand()/(float)(RAND_MAX/noise.freq_position_offset_target);
        if(freq > 1.0){

          offset_y = (float)rand()/(float)(RAND_MAX/noise.position_offset_target)-noise.position_offset_target/2.0;
        }else{
          offset_y = 0.0;
        }

        freq = (float)rand()/(float)(RAND_MAX/noise.freq_noise_horizon);
        float horizon_x = 0;
        float horizon_y = 0;
        if(freq > 1.0){
          float t = (float)rand()/(float)(RAND_MAX/noise.position_offset_target_horizon) - noise.position_offset_target_horizon/2.0;
          horizon_x = t*(1.0-cos_angle)*unit_x;
          horizon_y = t*(1.0-cos_angle)*unit_y;
        }


        result.obstacle_x[i] = obstacles[i].x + offset_x+horizon_x;
        result.obstacle_y[i] = obstacles[i].y + offset_y+horizon_y;


        if(distance < visible_radius_down_camera){
          freq = (float)rand()/(float)(RAND_MAX/noise.freq_angle_offset_down_camera);
          if(freq > 1.0){
            offset_angle = (float)rand()/(float)(RAND_MAX/(noise.angle_offset_down_camera*0.0174533))-(noise.angle_offset_down_camera*0.0174533)/2.0;
          }else{
            offset_angle = 0.0;
          }
        }else{
          freq = (float)rand()/(float)(RAND_MAX/noise.freq_angle_offset_target);
          if(freq > 1.0){
            offset_angle = (float)rand()/(float)(RAND_MAX/(noise.angle_offset_target*0.0174533))-(noise.angle_offset_target*0.0174533)/2.0;
          }else{
            offset_angle = 0.0;
          }

        }

        result.obstacle_q[i] = obstacles[i].q + offset_angle;
    }

    return result;
}



void add_history(sim_Command cmd, sim_State state, sim_Observed_State observed_state)
{
    if (HISTORY_LENGTH < History_Max_Length)
    {
        HISTORY_CMD[HISTORY_LENGTH] = cmd;
        HISTORY_STATE[HISTORY_LENGTH] = state;
        HISTORY_OBSERVED_STATE[HISTORY_LENGTH] = observed_state;
        HISTORY_LENGTH++;
    }
}

void world_to_ndc(r32 x_world, r32 y_world,
                  r32 *x_ndc, r32 *y_ndc)
{
    *x_ndc = (x_world - 10.0f) * NDC_SCALE_X;
    *y_ndc = (y_world - 10.0f) * NDC_SCALE_Y;
}
void vertex2f(r32 x, r32 y)
{
    r32 x_ndc, y_ndc;
    world_to_ndc(x, y, &x_ndc, &y_ndc);
    glVertex2f(x_ndc, y_ndc);
}

void color4f(Color color)
{
    glColor4f(color.r, color.g, color.b, color.a);
}

void draw_line(r32 x1, r32 y1, r32 x2, r32 y2)
{
    vertex2f(x1, y1);
    vertex2f(x2, y2);
}

void fill_square(r32 x1, r32 y1, r32 x2, r32 y2)
{
    r32 x1n, y1n, x2n, y2n;
    world_to_ndc(x1, y1, &x1n, &y1n);
    world_to_ndc(x2, y2, &x2n, &y2n);
    glVertex2f(x1n, y1n);
    glVertex2f(x2n, y1n);
    glVertex2f(x2n, y2n);
    glVertex2f(x2n, y2n);
    glVertex2f(x1n, y2n);
    glVertex2f(x1n, y1n);
}

void draw_painted_square(r32 x1, r32 y1, r32 x2, r32 y2)
{
    r32 x1n, y1n, x2n, y2n;
    world_to_ndc(x1, y1, &x1n, &y1n);
    world_to_ndc(x2, y2, &x2n, &y2n);
    glVertex2f(x1n, y1n);
    glVertex2f(x2n, y1n);
    glVertex2f(x2n, y2n);
}

void fill_circle(r32 x, r32 y, r32 r)
{
    int n = int(2* r *10.0);
    for (int i = 0; i < n; i++){
        float x1 = -r + 2*r*float(i)/( float(n));
        float y1 = sqrt(pow(r,2)-pow(x1,2));
        float x2 = x1+2*r/float(n);
        float y2 = -y1;


        fill_square(x1+x,y1+y,x2+x,y2+y);
    }
}

void draw_circle(r32 x, r32 y, r32 r, u32 n = 32)
{
    for (u32 i = 0; i < n; i++)
    {
        r32 t1 = TWO_PI * i / (r32)n;
        r32 t2 = TWO_PI * (i + 1) / (r32)n;
        r32 x1 = x + r*cos(t1);
        r32 y1 = y + r*sin(t1);
        r32 x2 = x + r*cos(t2);
        r32 y2 = y + r*sin(t2);
        draw_line(x1, y1, x2, y2);
    }
}

void draw_robot(sim_Robot robot)
{
    r32 x = robot.x;
    r32 y = robot.y;
    r32 l = 2*Sim_Robot_Radius;
    r32 q = robot.q;
    draw_circle(x, y, 0.5f*l);
    draw_line(x, y, x + l*cos(q), y + l*sin(q));
}

void draw_observed_robot(r32 x,  r32 y, r32 q)
{
    r32 l = 2*Sim_Robot_Radius;
    draw_circle(x, y, 0.5f*l);
    draw_line(x, y, x + l*cos(q), y + l*sin(q));
}

void draw_drone(float radius,float x,float y,bool flag_fancy_drone){
    glBegin(GL_LINES);
    float propel_radius = radius/3.0;
    if(! flag_fancy_drone){
         draw_circle(x,y,radius);
         glEnd();
    }else{
        // //propel 1
        radius = radius*2; // radius is the size of the undercarriage, radius*2 this will give a more realistic size of actual drone
        float propel_1_x = x + sin(PI/4.0)*radius;
        float propel_1_y = y + cos(PI/4.0)*radius;
        draw_line(propel_1_x-propel_radius,propel_1_y,propel_1_x+propel_radius,propel_1_y);
         draw_line(propel_1_x,propel_1_y-propel_radius,propel_1_x,propel_1_y+propel_radius);
        draw_circle(propel_1_x,propel_1_y,propel_radius);
    
        //propel 2
        float propel_2_x = x - sin(PI/4.0)*radius;
        float propel_2_y = y - cos(PI/4.0)*radius;
        draw_line(propel_2_x-propel_radius,propel_2_y,propel_2_x+propel_radius,propel_2_y);
        draw_line(propel_2_x,propel_2_y-propel_radius,propel_2_x,propel_2_y+propel_radius);
        draw_circle(propel_2_x,propel_2_y,propel_radius);


        //propel 3
        float propel_3_x = x + sin(PI/4.0)*radius;
        float propel_3_y = y - cos(PI/4.0)*radius;
        draw_line(propel_3_x-propel_radius,propel_3_y,propel_3_x+propel_radius,propel_3_y);
        draw_line(propel_3_x,propel_3_y-propel_radius,propel_3_x,propel_3_y+propel_radius);
       draw_circle(propel_3_x,propel_3_y,propel_radius);

        // //propel 4
        float propel_4_x = x - sin(PI/4.0)*radius;
        float propel_4_y = y + cos(PI/4.0)*radius;
        draw_line(propel_4_x-propel_radius,propel_4_y,propel_4_x+propel_radius,propel_4_y);
        draw_line(propel_4_x,propel_4_y-propel_radius,propel_4_x,propel_4_y+propel_radius);
        draw_circle(propel_4_x,propel_4_y,propel_radius);

        glEnd();
        glBegin(GL_TRIANGLES);
        //draw body
         fill_square(x-radius/4,y-radius*0.7,x+radius/4,y+radius*0.7);


         fill_square(x-radius/3,y+radius*0.8,x+radius/3,y+radius*0.6);
        fill_square(x-radius/3,y-radius*0.8,x+radius/3,y-radius*0.6);

        glEnd();
    }
    

    
}

void draw_planks(sim_Robot robot)
{
    r32 x = robot.x;
    r32 y = robot.y;
    r32 q = robot.q;
    r32 plank_angle = robot.plank_angle;
    robot_Internal internal = robot.internal;

    float plank_behind =  std::max((internal.time_since_last_reverse- Reverse_Length) * Robot_Speed,0.0f);
    float plank_ahead = std::min(internal.time_to_next_reverse * Robot_Speed,(Reverse_Interval- Reverse_Length) * Robot_Speed);


    draw_line(x, y, x + plank_ahead*cos(plank_angle), y + plank_ahead*sin(plank_angle));
    draw_line(x, y, x + plank_behind*cos(plank_angle-PI), y + plank_behind*sin(plank_angle-PI));
}

void draw_observed_plank(r32 x,r32 y,  r32 q ,r32 plank_angle,robot_Internal internal)
{

    float plank_behind =  std::max((internal.time_since_last_reverse- Reverse_Length) * Robot_Speed,0.0f);
    float plank_ahead = std::min(internal.time_to_next_reverse * Robot_Speed,(Reverse_Interval- Reverse_Length) * Robot_Speed);


    draw_line(x, y, x + plank_ahead*cos(plank_angle), y + plank_ahead*sin(plank_angle));
    draw_line(x, y, x + plank_behind*cos(plank_angle-PI), y + plank_behind*sin(plank_angle-PI));

}

struct FileData
{
    int length;
    u32 seed;
    sim_Command cmds[History_Max_Length];
};

bool write_history(char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (!f)
        return false;
    static FileData data;
    data.seed = STATE.seed;
    data.length = HISTORY_LENGTH;
    for (int i = 0; i < HISTORY_LENGTH; i++)
        data.cmds[i] = HISTORY_CMD[i];
    fwrite((char*)&data, 1, sizeof(data), f);
    fclose(f);
    return true;
}




bool read_history(char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        Printf("Failed to open file\n");
        return false;
    }
    static FileData data;
    fread((char*)&data, sizeof(data), 1, f);
    HISTORY_LENGTH = data.length;
    STATE = sim_init(data.seed);
    for (int i = 0; i < HISTORY_LENGTH; i++)
    {
        HISTORY_CMD[i] = data.cmds[i];
        STATE = sim_tick(STATE, HISTORY_CMD[i]);
        HISTORY_STATE[i] = STATE;
    }
    fclose(f);
    return true;
}

float heat_blue_color(float x){
    if(x<0.0){
        return 1.0;
    }else if(x>=0.0 && x < 0.25){
        return 1.0;
    }else if(x>= 0.25 && x<0.5){
        return (-4.0*(x-0.5));
    }else if(x>=0.5){
        return 0.0;
    }
}

float heat_green_color(float x){

    if(x<0.0){
        return 0.0;
    }else if(x>=0.0 && x < 0.25){
        return 4.0*x;
    }else if(x>= 0.25 && x<0.75){
        return 1.0;
    }else if(x>=0.75 && x < 1.0){
        return (-4.0*(x-1.0));
    }else{
        return 0.0;
    }
}

float heat_red_color(float x){
    if(x<0.5){
        return 0.0;
    }else if(x>=0.5 && x < 0.75){
        return (4.0*(x-0.5));
    }else if(x>= 0.75){
        return 1.0;
    }
}

float transform_heat_color(float x){
    return 0.1205*log(4000.0*x+1.0);
}


// Draw a formatted text string at window coordinate (x,y) measured from top-left
// Usage: DrawString(200, 300, "Hello world %.2f %d", 3.1415926f, 42);
void DrawString(int unique_id, float x, float y, float height, float width, const char* fmt, ...)
{
    float anchor_point_x = width/2;
    float anchor_point_y = height/2;
    y = 20.0-y; //reverse for correct plot
    y = y-10; //move to origo
    x = x - 10; // move to origo
    y = y*height/24; //scale
    x = x*height/24; //scale
    x = x+anchor_point_x ;//translate
    y = y+anchor_point_y; //translate
    //x = x+width/2; //center
    char name[1024];
    sprintf(name, "draw_string_%d", unique_id);
    va_list args;
    va_start(args, fmt);

    ImGui::SetNextWindowPos(ImVec2(x, y));
    ImGui::Begin(name, 0, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::TextV(fmt, args);
    ImGui::End();
    va_end(args);
}

void gui_tick(VideoMode mode, r32 gui_time, r32 gui_dt,int k)
{
    persist bool flag_grid     = false;
    persist bool flag_plank = false;
    persist bool flag_DrawDroneGoto     = true;
    persist bool flag_draw_observation     = false;
    persist bool flag_DrawDrone         = true;
    persist bool flag_DrawVisibleRegion = true;
    persist bool flag_DrawTargets       = true;
    persist bool flag_DrawObstacles     = true;
    persist bool flag_Paused            = false;
    persist bool flag_Recording         = false;
    persist bool flag_SetupRecord       = false;
    persist bool flag_probability_distribution = false;
    persist bool flag_custom_drone_text = false;
    persist bool flag_custom_target_text = false;
    persist bool flag_view_target_text = true;
    persist bool flag_view_drone_text = true;
    persist bool flag_send_perfect_data = true;
    persist bool flag_fancy_drone = false;
    persist int record_from = 0;
    persist int record_to = 0;
    persist int record_frame_skip = 1;
    persist int record_width = 0;
    persist int record_height = 0;
    persist float record_region_x = -1.0f;
    persist float record_region_y = -1.0f;
    persist float record_region_scale = 2.0f;
    persist jo_gif_t record_gif;

    persist int seek_cursor = 0;
    persist int selected_target = -1;

    persist Color color_Clear          = { 0.00f, 0.00f, 0.00f, 1.00f };
    persist Color color_Tiles          = { 0.20f, 0.35f, 0.46f, 0.66f };
    persist Color color_Grid           = { 0.00f, 0.00f, 0.00f, 1.00f };
    persist Color color_VisibleRegion  = { 1.0f, 1.0f, 1.0f, 0.10f };
    persist Color color_GreenLine      = { 0.10f, 1.00f, 0.20f, 1.00f };
    persist Color color_SelectedTarget = { 0.85f, 0.34f, 0.32f, 1.00f };
    persist Color color_Targets        = { 0.85f, 0.83f, 0.37f, 1.00f };
    persist Color color_Targets_prob   = { 0.00f, 0.00f, 0.00f, 1.00f };
    persist Color color_Obstacles      = { 0.43f, 0.76f, 0.79f, 1.00f };
    persist Color color_Drone          = { 0.05f, 0.05f, 0.05f, 1.0f };
    persist Color color_DroneGoto      = { 0.87f, 0.93f, 0.84f, 0.50f };
    persist Color color_Planks         = { 0.85f, 0.83f, 0.37f, 0.50f };


    persist Color color_transp_SelectedTarget = { 0.85f, 0.34f, 0.32f, 0.25f };
    persist Color color_transp_Targets        = { 0.85f, 0.83f, 0.37f, 0.25f };
    persist Color color_transp_Targets_prob   = { 0.00f, 0.00f, 0.00f, 0.25f };
    persist Color color_transp_Obstacles      = { 0.43f, 0.76f, 0.79f, 0.25f };
    persist Color color_transp_Drone          = { 0.05f, 0.05f, 0.05f, 0.25f };
    persist Color color_transp_Planks         = { 0.85f, 0.83f, 0.37f, 0.12f };

    persist Color white = { 1.0f, 1.0f, 1.0f, 1.0f };
    persist Color yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

    #define RGBA(C) C.r, C.g, C.b, C.a

    persist float send_timer = 0.0f;

    persist float send_interval = 0.2f;// In simulation time units


    persist float position_offset_target = 0.15;
    persist float freq_position_offset_target = 1.0;

    persist float position_offset_target_horizon =0.9;
    persist float freq_noise_horizon = 1.9;

    persist float percent_target_in_view = 0.9;

    persist float angle_offset_target = 180.0;
    persist float freq_angle_offset_target = 1.5;

    persist float position_offset_drone = 0.2;
    persist float freq_position_offset_drone = 1.5;

    persist float freq_angle_offset_down_camera = 1.5;
    persist float angle_offset_down_camera = 20.0;

    Noise_Object noise;
    noise.position_offset_target = position_offset_target;
    noise.freq_position_offset_target = freq_position_offset_target;

    noise.position_offset_target_horizon = position_offset_target_horizon;
    noise.freq_noise_horizon = freq_noise_horizon;

    noise.percent_target_in_view = percent_target_in_view;

    noise.angle_offset_target = angle_offset_target;
    noise.freq_angle_offset_target = freq_angle_offset_target;

    noise.position_offset_drone = position_offset_drone;
    noise.freq_position_offset_drone = freq_position_offset_drone;

    noise.angle_offset_down_camera = angle_offset_down_camera;
    noise.freq_angle_offset_down_camera = freq_angle_offset_down_camera;


    NDC_SCALE_X = (mode.height / (r32)mode.width) / 12.0f;
    NDC_SCALE_Y = 1.0f / 12.0f;

    if (flag_Recording || flag_SetupRecord)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        float Ax = 2.0f / record_region_scale;
        float Bx = -1.0f - Ax*record_region_x;
        float Ay = 2.0f / record_region_scale;
        float By = -1.0f - Ay*record_region_y;
        float modelview[] = {
            Ax,   0.0f, 0.0f, 0.0f,
            0.0f, Ay,   0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            Bx,   By,   0.0f, 1.0f
        };
        glLoadMatrixf(modelview);
    }
    else
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    if (!flag_Paused)
    {
        if (flag_Recording)
        {
            if (seek_cursor >= record_to)
            {
                flag_Paused = true;
                flag_Recording = false;
                seek_cursor = record_from;
                jo_gif_end(&record_gif);
            }
            else if (seek_cursor + record_frame_skip >= record_to)
            {
                // clamp to end
                seek_cursor = record_to;
            }
            else
            {
                seek_cursor += record_frame_skip;
            }
        }
        else if (seek_cursor < HISTORY_LENGTH-1)
        {
            seek_cursor++;
        }
        else
        {
            sim_Command cmd;

            if (!sim_recv_cmd(&cmd))
            {
                cmd.type = sim_CommandType_NoCommand;
                cmd.x = -1.0;
                cmd.y = -1.0;
                cmd.i = -1;


                for(int v = 0; v <20*20*pixels_each_meter*pixels_each_meter; v++){
                     cmd.heatmap[v] = HISTORY_CMD[HISTORY_LENGTH-1].heatmap[v];
                }
                for(int v = 0; v <100*Num_Targets; v++){
                     cmd.text[v] = HISTORY_CMD[HISTORY_LENGTH-1].text[v];
                }


                cmd.reward = -1.0;

            }
          /** if(cmd.type = sim_CommandType_NoCommand){
               cmd.x = -1.0;
               cmd.y = -1.0;
               cmd.i = -1;
           }
           else if(cmd.type = sim_CommandType_Track){
               cmd.x = -1.0;
               cmd.y = -1.0;
           }
           else if(cmd.type = sim_CommandType_Search){
               cmd.i = -1.0;
           }
           else if(cmd.type = sim_CommandType_LandOnTopOf){
               cmd.x = -1.0;
               cmd.y = -1.0;
           }
           else if(cmd.type = sim_CommandType_LandInFrontOf){
               cmd.x = -1.0;
               cmd.y = -1.0;
           }
           else if(cmd.type = sim_CommandType_Land){
               cmd.i = -1.0;
           }**/


            //for(int bit = 0; bit < pixels_each_meter*pixels_each_meter*20*20; bit++ )
            //{
            //  cmd.heatmap[bit] = 0.0;
            //}
            //for(int bit = 0; bit < 256*Num_Targets; bit++ )
            //{
            //  cmd.text[bit] = ;
            //}
            STATE = sim_tick(STATE, cmd);
            //add some noise to simulate perception

            sim_Observed_State observed_state = generate_noise(STATE,noise);
            sim_Observed_State perfect_data =  sim_observe_state(STATE);
            add_history(cmd, STATE, observed_state);
            seek_cursor = HISTORY_LENGTH-1;

            send_timer -= Sim_Timestep;
            if (send_timer <= 0.0f)
            {
                if(flag_send_perfect_data){
                  sim_send_state(&perfect_data);
                }else{
                  sim_send_state(&observed_state);
                }

                send_timer += send_interval;
            }
        }
    }

    sim_State draw_state = HISTORY_STATE[seek_cursor];
    sim_Observed_State observed = HISTORY_OBSERVED_STATE[seek_cursor];
    sim_Command cmd_state = HISTORY_CMD[seek_cursor];

    sim_Drone drone = draw_state.drone;
    //sim_Command cmd_state = HISTORY_CMD[seek_cursor];
    sim_Robot *robots = draw_state.robots;
    sim_Robot *targets = draw_state.robots;
    sim_Robot *obstacles = draw_state.robots + Num_Targets;

    if (flag_Recording || flag_SetupRecord)
    {
        glViewport(0, 0, record_width, record_height);
    }
    else
    {
        glViewport(0, 0, mode.width, mode.height);
    }

    glClearColor(RGBA(color_Clear));
    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(2.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw grid tiles
    glBegin(GL_TRIANGLES);
    {
        color4f(color_Tiles);
        for (int yi = 0; yi < 20; yi++)
        for (int xi = 0; xi < 20; xi++)
        {
            r32 x = xi*1.0f;
            r32 y = yi*1.0f;
            fill_square(x, y, x+1.0f, y+1.0f);
        }
    }
    // draw heat map
        if (flag_probability_distribution)
        {
            int iterations = pixels_each_meter * 20;
            double unit = 1.0 / float(pixels_each_meter);
            for(int yi = 0; yi < iterations; yi++){


                    for (int xi = 0; xi < iterations; xi++)
                    {


                        float value =  cmd_state.heatmap[yi*iterations + xi];

                        value = transform_heat_color(value);
                        float blue = heat_blue_color(value);
                        float green = heat_green_color(value);
                        float red = heat_red_color(value);

                        Color fading_color = { red , green, blue, 1.00f };
                        color4f(fading_color);
                        r32 x = xi * unit;
                        r32 y = yi* unit;
                        fill_square(x ,y, x+unit, y+unit);
                    }
            }


        }
    // draw visible region
    if (flag_DrawVisibleRegion)
    {
        color4f(color_VisibleRegion);

        fill_circle(drone.x, drone.y, compute_drone_view_radius(drone.z));
    }


    glEnd();

    glBegin(GL_LINES);
    {
        // draw grid lines
        if(flag_grid)
            {
            color4f(color_Grid);
            for (int i = 0; i <= 20; i++)
            {
                r32 x = (r32)i;
                draw_line(x, 0.0f, x, 20.0f);
                draw_line(0.0f, x, 20.0f, x);
            }
        }



        // draw green line
        color4f(color_GreenLine);
        draw_line(0.0f, 20.0f, 20.0f, 20.0f);


        // draw targets
        if (flag_DrawTargets)
        {
            if(flag_probability_distribution){
                color4f(color_Targets_prob);
                if(flag_draw_observation){
                  color4f(color_transp_Targets_prob);
                }
            }else{
                color4f(color_Targets);
                if(flag_draw_observation){
                  color4f(color_transp_Targets);
                }
            }
            for (int i = 0; i < Num_Targets; i++)
                draw_robot(targets[i]);
            if (selected_target >= 0)
            {
                color4f(color_SelectedTarget);
                if(flag_draw_observation){
                  color4f(color_transp_SelectedTarget);
                }
                draw_robot(targets[selected_target]);
            }
        }
        // draw Planks
        if (flag_plank)
        {
            color4f(color_Planks);
            if(flag_draw_observation){
              color4f(color_transp_Planks);
            }
            for (int i = 0; i < Num_Targets; i++){
                draw_planks(targets[i]);
            }
        }
        // draw obstacles
        if (flag_DrawObstacles)
        {
            color4f(color_Obstacles);
            if(flag_draw_observation){
              color4f(color_transp_Obstacles);
            }
            for (int i = 0; i < Num_Obstacles; i++)
                draw_robot(obstacles[i]);
        }

        // draw observed things
        if(flag_draw_observation){
          //draw observed targets
          if (flag_DrawTargets)
          {
              /**
              if(flag_probability_distribution){
                  color4f(color_Targets_prob);
              }else{
                  color4f(color_Targets);
              }**/
              color4f(white);
              for (int i = 0; i < Num_Targets; i++){

                if(observed.target_in_view[i]){
                  draw_observed_robot(observed.target_x[i],observed.target_y[i],observed.target_q[i]);
                }

              }
              if (selected_target >= 0)
              {
                  //color4f(color_SelectedTarget);
                  color4f(white);
                  draw_observed_robot(observed.target_x[selected_target],observed.target_y[selected_target],observed.target_q[selected_target]);
              }
          }
          // draw observed Planks
          if (flag_plank)
          {
              //color4f(color_Planks);
              color4f(white);
              for (int i = 0; i < Num_Targets; i++){
                if(observed.target_in_view[i]){
                  draw_observed_plank(observed.target_x[i],observed.target_y[i],  observed.target_q[i], observed.target_q[i], targets[i].internal);
                  }
              }
          }
          // draw observed obstacles
          if (flag_DrawObstacles)
          {
              //color4f(color_Obstacles);
              color4f(color_Obstacles);
              for (int i = 0; i < Num_Obstacles; i++)
                  draw_observed_robot(observed.obstacle_x[i],observed.obstacle_y[i],observed.obstacle_q[i]);
          }
        }




        // draw drone goto
        if (flag_DrawDroneGoto)
        {
            color4f(color_DroneGoto);
            draw_line(drone.xr - 0.3, drone.yr,  drone.xr + 0.3, drone.yr);
            draw_line(drone.xr, drone.yr - 0.3, drone.xr, drone.yr + 0.3);
        }

        // draw indicators of magnet or bumper activations
        for (int i = 0; i < Num_Targets; i++)
        {
            r32 x = targets[i].x;
            r32 y = targets[i].y;
            if (targets[i].action.was_bumped)
            {
                glColor4f(1.0f, 0.3f, 0.1f, 1.0f);
                draw_circle(x, y, 0.5f);
            }
            else if (targets[i].action.was_top_touched)
            {
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                draw_circle(x, y, 0.5f);
            }
        }
    }
    glEnd();



    // draw drone

    if (flag_DrawDrone)
    {
        //draw shadow of drone
        Color color_Shadow = { 0.00f, 0.00f, 0.00f, 0.05 + 0.5*pow((1.0  - drone.z / 3.0),2.0) };
        color4f(color_Shadow);
        draw_drone(Sim_Drone_Radius + drone.z/5.0,drone.x, drone.y,flag_fancy_drone);


        if(flag_draw_observation){
          //draw drone
          color4f(color_transp_Drone);
          draw_drone(Sim_Drone_Radius,drone.x,drone.y,flag_fancy_drone);
          //draw drone
          //color4f(color_transp_Drone);
          color4f(white);
          draw_drone(Sim_Drone_Radius,observed.drone_x,observed.drone_y,flag_fancy_drone);

        }else{
          //draw drone
          color4f(color_Drone);
          draw_drone(Sim_Drone_Radius,drone.x,drone.y,flag_fancy_drone);

        }


    }

    // TODO: Change capture res?
    if (flag_Recording)
    {
        static unsigned char capture_data[1024*1024*4];
        Assert(sizeof(capture_data) >=
               record_width*record_height*4);
        glReadPixels(0, 0,
                     record_width,
                     record_height,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     capture_data);

        jo_gif_frame(&record_gif, capture_data,
                     2, false);
    }

    ImGui_ImplSdl_NewFrame(mode.window);




    // DRAW FLAGS
    if (ImGui::CollapsingHeader("Rendering"))
    {
        ImGui::Checkbox("Probability distribution", &flag_probability_distribution);
        ImGui::Checkbox("View target text", &flag_view_target_text);
        ImGui::Checkbox("View drone text", &flag_view_drone_text);
        ImGui::Checkbox("Custom drone text", &flag_custom_drone_text);
        ImGui::Checkbox("Custom target text", &flag_custom_target_text);
        ImGui::Checkbox("Grid", &flag_grid);
        ImGui::Checkbox("Plank", &flag_plank);
        ImGui::Checkbox("View noisy observation", &flag_draw_observation);
        ImGui::Checkbox("Drone goto", &flag_DrawDroneGoto);
        ImGui::Checkbox("Draw drone", &flag_DrawDrone);
        ImGui::Checkbox("Fancy drone", &flag_fancy_drone);
        ImGui::Checkbox("Visible down area", &flag_DrawVisibleRegion);
        ImGui::Checkbox("Ground robots", &flag_DrawTargets);
        ImGui::Checkbox("Obstacles", &flag_DrawObstacles);


    } // END DRAW FLAGS

    // COLORS
    if (ImGui::CollapsingHeader("Colors"))
    {
        ImGui::ColorEdit4("Clear", &color_Clear.r);
        ImGui::ColorEdit4("Grid", &color_Grid.r);
        ImGui::ColorEdit4("VisibleRegion", &color_VisibleRegion.r);
        ImGui::ColorEdit4("GreenLine", &color_GreenLine.r);
        ImGui::ColorEdit4("Targets", &color_Targets.r);
        ImGui::ColorEdit4("Obstacles", &color_Obstacles.r);
        ImGui::ColorEdit4("Drone", &color_Drone.r);
        ImGui::ColorEdit4("DroneGoto", &color_DroneGoto.r);
    } // END COLORS

    // REWIND HISTORY
    if (ImGui::CollapsingHeader("Seek##header"))
    {
        ImGui::SliderInt("FPS##bar", &fps_lock, 1, 90);
        ImGui::Checkbox("Paused", &flag_Paused);
        ImGui::SliderInt("Seek##bar", &seek_cursor, 0, HISTORY_LENGTH-1);
        ImGui::InputInt("Seek frame", &seek_cursor);
        if (seek_cursor < 0) seek_cursor = 0;
        if (seek_cursor > HISTORY_LENGTH-1) seek_cursor = HISTORY_LENGTH-1;
        ImGui::Text("Time: %.2f seconds", (seek_cursor+1) * Sim_Timestep);
    } // END REWIND HISTORY

    // ROBOTS
    if (ImGui::CollapsingHeader("Robots"))
    {
        ImGui::Columns(4, "RobotsColumns");
        ImGui::Separator();
        ImGui::Text("ID"); ImGui::NextColumn();
        ImGui::Text("X"); ImGui::NextColumn();
        ImGui::Text("Y"); ImGui::NextColumn();
        ImGui::Text("Angle"); ImGui::NextColumn();
        ImGui::Separator();
        for (int i = 0; i < Num_Targets; i++)
        {
            char label[32];
            sprintf(label, "%02d", i);
            if (ImGui::Selectable(label, selected_target == i, ImGuiSelectableFlags_SpanAllColumns))
                selected_target = i;
            ImGui::NextColumn();
            ImGui::Text("%.2f", robots[i].x); ImGui::NextColumn();
            ImGui::Text("%.2f", robots[i].y); ImGui::NextColumn();
            ImGui::Text("%.2f", robots[i].q); ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();
    }
    else
    {
        selected_target = -1;
    } // END ROBOTS



    // noise generator
    if (ImGui::CollapsingHeader("Noise generator"))
    {

      ImGui::TextWrapped("Targets: Average noise offset (meters) " );
      ImGui::SliderFloat("Noise", &position_offset_target,  0.0f, 5.0f);
      ImGui::SliderFloat("Frequency", &freq_position_offset_target,0.0f, 5.0f);
      ImGui::TextWrapped(" ");

      ImGui::TextWrapped("Targets: Average noise offset pushed ");
      ImGui::TextWrapped("         into horizon (meters)");
      ImGui::SliderFloat("Noise ", &position_offset_target_horizon,0.0f, 5.0f);
      ImGui::SliderFloat("Frequency ", &freq_noise_horizon,0.0f, 5.0f);
      ImGui::TextWrapped(" ");

      ImGui::TextWrapped("Targets: Percent of time observed");
      ImGui::SliderFloat("Percent", &percent_target_in_view,0.0f, 1.0f);
      ImGui::TextWrapped(" ");

      ImGui::TextWrapped("Targets: Direction noise (degrees)");
      ImGui::SliderFloat("Noise  ", &angle_offset_target,0.0f, 360.0f);
      ImGui::SliderFloat("Frequency  ", &freq_angle_offset_target,0.0f, 5.0f);
      ImGui::TextWrapped(" ");

      ImGui::TextWrapped("Targets: Direction noise when spotted by");
      ImGui::TextWrapped("         down pointing camera (degrees)");
      ImGui::SliderFloat("Noise   ", &angle_offset_down_camera,0.0f, 360.0f);
      ImGui::SliderFloat("Frequency   ", &freq_angle_offset_down_camera,0.0f, 5.0f);
      ImGui::TextWrapped(" ");

      ImGui::TextWrapped("Drone: Average offset (meters)");
      ImGui::SliderFloat("Noise    ", &position_offset_drone,0.0f, 5.0f);
      ImGui::SliderFloat("Frequency    ", &freq_position_offset_drone,0.0f, 5.0f);




        ImGui::Separator();
    } // END noise

    // COMMUNICATION
    if (ImGui::CollapsingHeader("Communication"))
    {
        ImGui::Checkbox("Send perfect observations", &flag_send_perfect_data);
        ImGui::TextWrapped("The rate at which the state is "
                           "sent can be changed using this slider. "
                           "The slider value represents the time "
                           "interval (in simulation time) "
                           "between each send.");
        ImGui::SliderFloat("Send interval", &send_interval,
                           Sim_Timestep, 1.0f);
        ImGui::Separator();

        ImGui::Text("Last 10 non-trivial commands received:");
        ImGui::Columns(6, "CommunicationColumns");
        ImGui::Separator();
        ImGui::Text("Time"); ImGui::NextColumn();
        ImGui::Text("type"); ImGui::NextColumn();
        ImGui::Text("x"); ImGui::NextColumn();
        ImGui::Text("y"); ImGui::NextColumn();
        ImGui::Text("i"); ImGui::NextColumn();
        ImGui::Text("Reward"); ImGui::NextColumn();
        ImGui::Separator();
        int count = 0;
        for (int i = 0; count < 10 && i <= seek_cursor; i++)
        {
            sim_State state_i = HISTORY_STATE[seek_cursor-i];
            sim_Command cmd_i = HISTORY_CMD[seek_cursor-i];
            if (cmd_i.type == sim_CommandType_NoCommand)
                continue;
            char label[32];
            sprintf(label, "%.2f", state_i.elapsed_time);
            ImGui::Selectable(label, false,
                              ImGuiSelectableFlags_SpanAllColumns);
            ImGui::NextColumn();
            switch (cmd_i.type)
            {
                case sim_CommandType_NoCommand:
                {
                    ImGui::Text("Nothing"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                } break;
                case sim_CommandType_LandInFrontOf:
                {
                    ImGui::Text("Land 180"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("%d", cmd_i.i); ImGui::NextColumn();
                    ImGui::Text("%f", cmd_i.reward); ImGui::NextColumn();
                } break;
                case sim_CommandType_Land:
                {
                    ImGui::Text("Land at"); ImGui::NextColumn();
                    ImGui::Text("%.2f", cmd_i.x); ImGui::NextColumn();
                    ImGui::Text("%.2f", cmd_i.y); ImGui::NextColumn();
                    ImGui::Text("%d", cmd_i.i); ImGui::NextColumn();
                    ImGui::Text("%f", cmd_i.reward); ImGui::NextColumn();
                } break;
                case sim_CommandType_LandOnTopOf:
                {
                    ImGui::Text("Land 45"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("%d", cmd_i.i); ImGui::NextColumn();
                    ImGui::Text("%f", cmd_i.reward); ImGui::NextColumn();
                } break;
                case sim_CommandType_Track:
                {
                    ImGui::Text("Track"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("%d", cmd_i.i); ImGui::NextColumn();
                    ImGui::Text("%f", cmd_i.reward); ImGui::NextColumn();
                } break;
                case sim_CommandType_Search:
                {
                    ImGui::Text("Search"); ImGui::NextColumn();
                    ImGui::Text("%.2f", cmd_i.x); ImGui::NextColumn();
                    ImGui::Text("%.2f", cmd_i.y); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("%f", cmd_i.reward); ImGui::NextColumn();
                } break;
                case sim_CommandType_Debug:
                {
                    ImGui::Text("Debug"); ImGui::NextColumn();
                    ImGui::Text("%.2f", cmd_i.x); ImGui::NextColumn();
                    ImGui::Text("%.2f", cmd_i.y); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                    ImGui::Text("-"); ImGui::NextColumn();
                } break;
            }
            count++;
        }
        ImGui::Columns(1);
        ImGui::Separator();
    } // END COMMUNICATION

    // RECORDING GIFS
    if (ImGui::CollapsingHeader("Recording"))
    {
        flag_SetupRecord = true;
        if (ImGui::Button("Mark frame as begin"))
        {
            record_from = seek_cursor;
        }
        ImGui::SameLine();
        ImGui::Text("Record from: %d", record_from);
        if (ImGui::Button("Mark frame as end"))
        {
            record_to = seek_cursor;
        }
        ImGui::SameLine();
        ImGui::Text("Record to: %d", record_to);
        ImGui::InputInt("Frame skip", &record_frame_skip);

        ImGui::InputInt("Record width", &record_width);
        ImGui::InputInt("Record height", &record_height);
        if (record_width <= 0) record_width = mode.width;
        if (record_height <= 0) record_height = mode.height;

        ImGui::SliderFloat("Record x", &record_region_x, -1.0f, 1.0f);
        ImGui::SliderFloat("Record y", &record_region_y, -1.0f, 1.0f);
        ImGui::SliderFloat("Record scale", &record_region_scale, 0.0f, 2.0f);

        if (ImGui::Button("Start recording") && !flag_Recording)
            ImGui::OpenPopup("Save recording as?");
        if (ImGui::BeginPopupModal("Save recording as?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            persist char filename[1024];
            persist bool init_filename = true;
            if (init_filename)
            {
                sprintf(filename, "recording%u.gif", STATE.seed);
                init_filename = false;
            }
            ImGui::InputText("Filename", filename, sizeof(filename));
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120,0)))
            {
                flag_Recording = true;
                flag_Paused = false;
                seek_cursor = record_from-1;
                record_gif = jo_gif_start(filename, (short)record_width, (short)record_height, 0, 32);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120,0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Stop recording") && flag_Recording)
        {
            flag_Recording = false;
            flag_Paused = true;
            jo_gif_end(&record_gif);
        }

        if (record_from < 0) record_from = 0;
        if (record_from > HISTORY_LENGTH-1) record_from = HISTORY_LENGTH-1;
        if (record_to < record_from) record_to = record_from;
        if (record_to > HISTORY_LENGTH-1) record_to = HISTORY_LENGTH-1;
        if (record_frame_skip < 1) record_frame_skip = 1;
        ImGui::Separator();
    }
    else
    {
        flag_SetupRecord = false;
    } // END RECORDING GIFS

    // RESET AND SET SEED
    persist int custom_seed = 0;
    if (ImGui::Button("Reset"))
    {
        if (custom_seed > 0)
            STATE = sim_init((u32)custom_seed);
        else
            STATE = sim_init((u32)get_tick());
        HISTORY_LENGTH = 0;
        sim_Command cmd;
        cmd.type = sim_CommandType_NoCommand;
        add_history(cmd, STATE, generate_noise(STATE,noise));
    }
    ImGui::SameLine();
    ImGui::InputInt("Seed", &custom_seed);
    // END RESET AND SET SEED

    // SAVE SIMULATION
    if (ImGui::Button("Save.."))
        ImGui::OpenPopup("Save as?");
    if (ImGui::BeginPopupModal("Save as?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        persist char filename[1024];
        persist bool init_filename = true;
        if (init_filename)
        {
            sprintf(filename, "simulation%u", STATE.seed);
            init_filename = false;
        }
        ImGui::InputText("Filename", filename, sizeof(filename));
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120,0)))
        {
            write_history(filename);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120,0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    } // END SAVE SIMULATION

    ImGui::SameLine();

    // SAVE SINGLE SNAPSHOT
    persist bool init_snapshot_filename = true;
    if (ImGui::Button("Save snapshot.."))
        ImGui::OpenPopup("Save snapshot as?");
    if (ImGui::BeginPopupModal("Save snapshot as?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        persist char filename[1024];
        ImGui::TextWrapped("The filename is relative to the executable,"
                           "unless you write an absolute path.");
        if (init_snapshot_filename)
        {
            sprintf(filename, "snapshot%u-%u", STATE.seed, seek_cursor);
            init_snapshot_filename = false;
        }
        ImGui::InputText("Filename", filename, sizeof(filename));
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120,0)))
        {
            sim_Observed_State snapshot =
                sim_observe_state(HISTORY_STATE[seek_cursor]);
            //printf("%.2f\n", snapshot.obstacle_q[0]);
            sim_write_snapshot(filename, snapshot);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120,0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    else
    {
        init_snapshot_filename = true;
    } // END SAVE SIMULATION

    ImGui::SameLine();

    // LOAD SIMULATION
    if (ImGui::Button("Load.."))
        ImGui::OpenPopup("Load file?");
    if (ImGui::BeginPopupModal("Load file?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        persist char filename[1024];
        persist bool init_filename = true;
        if (init_filename)
        {
            sprintf(filename, "simulation%u", STATE.seed);
            init_filename = false;
        }
        ImGui::InputText("Filename", filename, sizeof(filename));
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120,0)))
        {
            read_history(filename);
            seek_cursor = 0;
            flag_Paused = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120,0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    } // END LOAD SIMULATION


    //GET LAST COMMAND


    sim_Command cmd_i ;
    int found_cmd_at_i = 0;
    int found_done_at_i = 0;

    //search after last command
    for(int i = seek_cursor; i>= 0; i-- ){
        cmd_i = HISTORY_CMD[i];
        if(cmd_i.type != sim_CommandType_NoCommand){
                found_cmd_at_i = i;
                break;
            }

    }
    //search after last cmd_done
    for(int i = seek_cursor; i>= 0; i-- ){
        sim_State state_i = HISTORY_STATE[i];
        if(state_i.drone.cmd_done){
                found_done_at_i = i;
                break;
            }

    }
    if(found_done_at_i>=found_cmd_at_i){
        cmd_i.type = sim_CommandType_NoCommand;
        cmd_i.i = -1;
        cmd_i.x = -1;
        cmd_i.y = -1;
    }
    char cmd_text[128];
    switch (cmd_i.type)
    {
        case sim_CommandType_NoCommand:
        {
        snprintf(cmd_text, sizeof cmd_text, "%s","NoCommand");

        } break;
        case sim_CommandType_LandInFrontOf:
        {
            snprintf(cmd_text, sizeof cmd_text, "%s","LandInFrontOf");
        } break;
        case sim_CommandType_LandOnTopOf:
        {
           snprintf(cmd_text, sizeof cmd_text, "%s","LandOnTopOf");
        } break;
        case sim_CommandType_Track:
        {
            snprintf(cmd_text, sizeof cmd_text, "%s","Track");
        } break;
        case sim_CommandType_Search:
        {
            snprintf(cmd_text, sizeof cmd_text, "%s","Search");
        } break;
        case sim_CommandType_Land:
        {
           snprintf(cmd_text, sizeof cmd_text, "%s","Land");
        } break;
    }
    //DRAW TEXT


    //Draw target texts
    if (flag_view_target_text){
        if(flag_custom_target_text){

            for (int i = 0; i < Num_Targets; i++){
                char some_text[Num_max_text_length];
                some_text[0] = 'i';
                some_text[1] = ':';
                some_text[2] = '0'+i;
                some_text[3] = ' ';
                int k = 4;
                for(int j = i*Num_max_text_length+Num_max_text_length; j < i*Num_max_text_length+Num_max_text_length*2; j++){
                    if(cmd_i.text[j] == '$'){
                        some_text[k] = '\0';
                        break;
                    }
                    some_text[k] = cmd_i.text[j];
                    k++;
                }
                DrawString(i,targets[i].x, targets[i].y,mode.height,mode.width, some_text);

            }



        }else{
            for (int i = 0; i < Num_Targets; i++){
                char str[1024];
                if(cmd_i.i == i && (cmd_i.type == sim_CommandType_LandInFrontOf || cmd_i.type == sim_CommandType_LandOnTopOf || cmd_i.type == sim_CommandType_Track || cmd_i.type == sim_CommandType_Land )){
                    snprintf(str, sizeof str, "%s%d%s%s", "i:", i, " ", cmd_text);

                    DrawString(i,targets[i].x, targets[i].y,mode.height,mode.width, str);

                }else{
                    snprintf(str, sizeof str, "%s%d", "i:", i);
                    DrawString(i,targets[i].x, targets[i].y,mode.height,mode.width, str);
                }
            }
        }



    }
    //Draw drone text
    if (flag_view_drone_text){
        if(flag_custom_drone_text){
            char some_text[Num_max_text_length];
            for(int i = 0; i < Num_max_text_length; i++){
                if(cmd_i.text[i] == '$'){
                    some_text[i] = '\0';
                    break;
                }
                some_text[i] = cmd_i.text[i];
            }
            DrawString(999,drone.x, drone.y,mode.height,mode.width, some_text);
        }else{
            char more_info[1024];
            if(cmd_i.type == sim_CommandType_NoCommand){
                snprintf(more_info, sizeof more_info, "%s%.1f%s", "Alt:",drone.z," NoCommand!");

            }else{
                snprintf(more_info, sizeof more_info, "%s%.1f%s%s%s%.1f%s%.1f%s%d", "Alt:",drone.z," ",cmd_text," at x:",cmd_i.x," y:",cmd_i.y," i:",cmd_i.i);
            }
            DrawString(999,drone.x, drone.y,mode.height,mode.width, more_info);
        }




    }
    ImGui::Render();
} // END gui_tick

int main(int argc, char *argv[])
{
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        Printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    VideoMode mode = {};
    mode.width = 800;
    mode.height = 600;
    mode.gl_major = 1;
    mode.gl_minor = 5;
    mode.double_buffer = 1;
    mode.depth_bits = 24;
    mode.stencil_bits = 8;
    mode.multisamples = 0; // Values over 0 will make Ubuntu 17+ fail
    mode.swap_interval = 0;
    mode.fps_lock = fps_lock;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, mode.gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mode.gl_minor);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,          mode.double_buffer);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,            mode.depth_bits);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,          mode.stencil_bits);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,    mode.multisamples>0?1:0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,    mode.multisamples);

    mode.window = SDL_CreateWindow(
        "World Simulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        mode.width, mode.height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!mode.window)
    {
        Printf("Failed to create a window: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(mode.window);

    // Note: This must be set on a valid context
    SDL_GL_SetSwapInterval(mode.swap_interval);

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &mode.gl_major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &mode.gl_minor);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER,          &mode.double_buffer);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE,            &mode.depth_bits);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE,          &mode.stencil_bits);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES,    &mode.multisamples);
    mode.swap_interval = SDL_GL_GetSwapInterval();

    sim_init_msgs(false);

    ImGui_ImplSdl_Init(mode.window);

    STATE = sim_init((u32)get_tick());
    HISTORY_LENGTH = 0;

    bool running = true;
    u64 initial_tick = get_tick();
    u64 last_frame_t = initial_tick;
    r32 elapsed_time = 0.0f;
    r32 delta_time = 1.0f / 10.0f;
    int k = 0;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdl_ProcessEvent(&event);
            switch (event.type)
            {
                case SDL_WINDOWEVENT:
                {
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
                            Printf("Window %d size changed to %dx%d\n",
                                    event.window.windowID,
                                    event.window.data1,
                                    event.window.data2);
                            mode.width = event.window.data1;
                            mode.height = event.window.data2;
                        } break;
                    }
                } break;

                case SDL_QUIT:
                {
                    running = false;
                } break;
            }
        }
        k++;
        gui_tick(mode, elapsed_time, delta_time,k);
        SDL_GL_SwapWindow(mode.window);

        delta_time = time_since(last_frame_t);
        mode.fps_lock = fps_lock;
        if (mode.fps_lock > 0)
        {
            r32 target_time = 1.0f / (r32)mode.fps_lock;
            r32 sleep_time = target_time - delta_time;
            if (sleep_time >= 0.01f)
                SDL_Delay((u32)(sleep_time * 1000.0f));
            delta_time = time_since(last_frame_t);
        }
        last_frame_t = get_tick();
        elapsed_time = time_since(initial_tick);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            Printf("An error occurred: %s\n", gl_error_message(error));
            running = false;
        }
    }

    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(mode.window);
    SDL_Quit();

    return 0;
}
