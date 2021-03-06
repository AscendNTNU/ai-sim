// Simulator for IARC mission 7A - v0.01 - public domain
// http://www.ascendntnu.no/
//
// Written by Simen Haugo in 2016, last updated in 2016
//
// LICENSE
// =======================================================================
//   This software is in the public domain. Where that dedication is not
//   recognized, you are granted a perpetual, irrevocable license to copy,
//   distribute, and modify this file as you see fit.
//
// No warranty for any purpose is expressed or implied by the author (nor
// by Ascend NTNU). Report bugs and send enhancements to the author.
//
// HOW TO COMPILE
// =======================================================================
// This file contains both the header file and the implementation file.
// To compile, insert the following in A SINGLE source file in your project
//
//     #define SIM_IMPLEMENTATION
//     #include "sim.h"
//
// You may otherwise include this file as you would include a traditional
// header file.
//
// HISTORY
// =======================================================================
// 18. jan 2016: Exposing the internal state for now
//
//
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef SIM_HEADER_INCLUDE
#define SIM_HEADER_INCLUDE
#define Num_Obstacles (4)  // Number of robots with pole
#define Num_Targets   (10) // Number of robots without pole
#define Num_Robots    (Num_Obstacles + Num_Targets)
#define Num_max_text_length (128) // Maximum number of bytes for each target text



#define pixels_each_meter (1) //for heatmap

enum sim_CommandType
{
    sim_CommandType_NoCommand = 0,   // continue doing whatever you are doing
    sim_CommandType_LandOnTopOf,     // trigger one 45 deg turn of robot (i)
    sim_CommandType_LandInFrontOf,   // trigger one 180 deg turn of robot (i)
    sim_CommandType_Track,           // follow robot (i) at a constant height
    sim_CommandType_Search,          // ascend to AverageFlyingHeight and go to (x, y)
    sim_CommandType_TakeOff,         // ascend to AverageFlyingHeight meters.
    sim_CommandType_Land,
    sim_CommandType_Debug
};

struct sim_Command
{
    sim_CommandType type;
    float x;
    float y;
    int i; //if sim_CommandType_Land is used, should only be applied for debug purposes
    float heatmap[pixels_each_meter*pixels_each_meter*20*20];
    char text[Num_max_text_length*Num_Targets+Num_max_text_length];

    float reward;

};

struct sim_Observed_State
{
    float elapsed_time;
    float drone_x;
    float drone_y;
    float drone_z;
    bool  drone_cmd_done;
    int num_Targets;

    bool  target_in_view[Num_Targets];
    bool  target_reversing[Num_Targets];
    bool  target_removed[Num_Targets];
    float target_reward[Num_Targets];

    float target_x[Num_Targets];
    float target_y[Num_Targets];
    float target_q[Num_Targets];

    float obstacle_x[Num_Obstacles];
    float obstacle_y[Num_Obstacles];
    float obstacle_q[Num_Obstacles];
};

struct sim_State;

sim_State          sim_tick(sim_State, sim_Command);
sim_State          sim_init(unsigned int);
sim_Observed_State sim_observe_state(sim_State);

sim_Observed_State sim_load_snapshot(char*);
void               sim_write_snapshot(char*, sim_Observed_State);

// sim_tick is advances the world state (given as an argument) by Sim_Timestep
// seconds of real time, and returns the new state.
//
// sim_init returns a default state with the robots configured according to
// the IARC rules, and with a specified seed for the random number generator.
//
// sim_observe_state takes a full state description and returns what we think
// is an appropriate "handicapped" state vector. i.e. an approximation of what
// we might get from computer vision and object tracking.
//
// If you want to store an observed state to file, or load it from a file,
// you can use sim_load/write_snapshot.




// ***********************************************************************
//                        Tweakable simulation parameters
// ***********************************************************************

#define Sim_Timestep (1.0f / 10.0f)       // Simulation timestep

// Note:
// Increasing Sim_Timestep will decrease the accuracy of the simulation,
// and may cause weird bugs like missed collisions. If you would like to
// simulate ahead a given number of seconds, I would suggest to keep the
// timestep low and run it in a for loop for n=Interval/Sim_Timestep
// iterations.

#define Sim_Drone_View_Radius (2.5f)      // Radius in meters of the circle
                                          // around the drone where robots
                                          // are considered visible.

#define Sim_Drone_Target_Proximity (0.1f) // How many meters the drone can
                                          // be near to a robot, before it
                                          // is considered to be above it.

#define Sim_Drone_Goto_Proximity (0.1f)   // How many meters the drone can
                                          // near a point in the world before
                                          // it is considered to have reached
                                          // it.
#define Sim_Drone_Radius (0.2f)           // How large the drone is

#define Sim_LandOnTopOf_Time (2.0f)       // How many seconds it should take
                                          // to complete a LandOnTopOf command
                                          // after the drone gets close enough.
#define Sim_Landing_Time (2.0f)           //How long time it should take to land

#define Sim_Take_Off_Time (2.0f)           //How long time it should take to take off


#define Sim_Average_Flying_Height (3.0f)   //The height the drone is flying at when not on ground

#define Sim_LandInFrontOf_Time (2.0f)     // How many seconds it should take
                                          // to complete a LandInFrontOf command
                                          // after the drone gets close enough.

#define Sim_Target_Removal_Margin (0.5f)  // How many meters a target can
                                          // drive behind an edge, before it
                                          // is removed.

#define Sim_Robot_Wheel_Distance (0.5f)   // Meters between iRobot wheels (correct value is 25cm but 0.5m calculates right turn time and so on)

#define Sim_Robot_Radius (0.17)            // Radius of iRobot roomba (ground robot)

#define Sim_Target_Init_Radius (1.0f)     // Initial target spawn radius

#define Sim_Obstacle_Init_Radius (5.0f)   // Initial obstacle spawn radius

#define Sim_Robot_Height (0.2f)         // The height of the ground robot

#define Sim_View_Angle (0.6)          //View angle to camera in radians



// ***********************************************************************
//                        Semi-implementation details for
//                             specially interested
// ***********************************************************************

// The world-space coordinate system is defined as follows:
// The red line is considered the x-axis, while the left
// edge connecting the red and green line is considered the
// y-axis. The z-axis is considered out of the screen, following
// the right hand rule of orthogonal frames. The unit is meters.
// (0,0) is the lower-left line intersection, (20,20) is the upper
// right line intersection.
//
//         x=0        x=20
//         y=20       y=20
//          +----------+                    ^ +y
//          |  GREEN   |                    |
//          |          |                    |
//          |          |                    |
//          |   RED    |                    |
//          +----------+                    +-------> +x
//         x=0        x=20
//         y=0        y=0
struct sim_World_Pos
{
    float x;
    float y;
};

// Angle, in radians, relative to the x-axis,
// positive counter-clockwise, wrapped to the
// half-open interval [0, 2xPi).
typedef float sim_World_Angle;

// Number of seconds ticked since the sim was intialized
typedef float sim_Time;

enum robot_State
{
    Robot_Start,
    Robot_ObstacleWait,
    Robot_ObstacleRun,
    Robot_ObstacleCollision,
    Robot_TargetWait,
    Robot_TargetRun,
    Robot_TrajectoryNoise,
    Robot_Reverse,
    Robot_TargetCollision,
    Robot_TopTouch
};

struct robot_Internal
{
    sim_Time begin_noise;
    sim_Time begin_reverse;
    sim_Time begin_top_touch;

    sim_Time last_noise;
    sim_Time last_reverse;
    sim_Time time_since_last_reverse;
    sim_Time time_to_next_reverse;
    int reverse_count;
    bool initialized;
};

struct robot_Event
{
    bool is_run_sig;
    bool is_wait_sig;
    bool is_top_touch;
    bool is_bumper;
    bool target_switch_pin;
    sim_Time elapsed_time;
};

struct robot_Action
{
    // This is the speed at the rim of the wheel (in contact with the ground).
    // The associated angular velocity, assuming no friction, is given by the
    // relationship w = vr, where
    //   w := Angular speed
    //   v := Linear speed
    //   r := Wheel radius
    float left_wheel;
    float right_wheel;
    bool red_led;
    bool green_led;

    bool was_top_touched;
    bool was_bumped;
};

struct sim_Robot
{
    robot_State state;
    robot_Internal internal;
    robot_Action action;

    float x;
    float y;
    sim_World_Angle q;
    sim_World_Angle plank_angle;

    // Physical parameters
    float L;  // distance between wheels (m)
    float vl; // left-wheel speed (m/s)
    float vr; // right-wheel speed (m/s)

    float forward_x;
    float forward_y;

    bool removed;
    int reward;
};

struct sim_Drone
{
    float x;
    float y;
    float z;
    float xr;
    float yr;
    float v_max;
    sim_Command cmd;
    bool cmd_done;
    bool on_ground;
    bool landing;
    bool ascending;
    float land_timer;
};

struct sim_State
{
    // Random-number-generator state
    unsigned int seed;
    unsigned int xor128_x;
    unsigned int xor128_y;
    unsigned int xor128_z;
    unsigned int xor128_w;

    sim_Time elapsed_time;
    sim_Drone drone;
    sim_Robot robots[Num_Robots];
};

#endif // SIM_HEADER_INCLUDE

// ***********************************************************************
//                        Implementation starts here
// ***********************************************************************
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifdef SIM_IMPLEMENTATION
#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef TWO_PI
#define TWO_PI 6.28318530718f
#endif

// Global variables
static sim_State *INTERNAL;
static sim_Robot *ROBOTS;
static sim_Robot *OBSTACLES;
static sim_Robot *TARGETS;
static sim_Drone *DRONE;

// This algorithm has a maximal period of 2^128 − 1.
// https://en.wikipedia.org/wiki/Xorshift
static void
xor128(unsigned int *in_out_x,
       unsigned int *in_out_y,
       unsigned int *in_out_z,
       unsigned int *in_out_w)
{
    unsigned int x = *in_out_x;
    unsigned int y = *in_out_y;
    unsigned int z = *in_out_z;
    unsigned int w = *in_out_w;
    unsigned int t = x ^ (x << 11);
    *in_out_x = y;
    *in_out_y = z;
    *in_out_z = w;
    *in_out_w = w ^ (w >> 19) ^ t ^ (t >> 8);
}

static unsigned int
_xor128()
{
    xor128(&INTERNAL->xor128_x,
           &INTERNAL->xor128_y,
           &INTERNAL->xor128_z,
           &INTERNAL->xor128_w);
    unsigned int result = INTERNAL->xor128_w;
    return result;
}

static int
random_0_64()
{
    return _xor128() % 65;
}

//used for returning a text which can be used for debug. The first string is the text which
//goes into the text field of the drone.
//The next is the groundrobot texts starting with 0 at the first position, and looping up to Num_Targets
static void
get_char_text(char cmd_text[Num_max_text_length+Num_max_text_length*Num_Targets], const char*  drone_text,  char const * ground_robots[],int size=Num_Targets)
{

    for(int i = 0; i < Num_max_text_length*Num_Targets+Num_max_text_length; i++){
        cmd_text[i] = '$';
    }
    for(int i = 0; i < strlen(drone_text); i++){
        cmd_text[i] = drone_text[i];
    }

    for (int i = 0; i < size; i++) {
        char str[strlen(ground_robots[i])];
        strncpy(str,ground_robots[i],strlen(ground_robots[i]));

        int k = 0;
        str[strlen(str)] = '$';

        for (int bit = i*Num_max_text_length+Num_max_text_length; bit < i*Num_max_text_length+Num_max_text_length*2; bit++){
             if(k>=strlen(str)){
                break;
             }
             cmd_text[bit] = str[k];
             k++;

        }
     }

}

// ***********************************************************************
// What follows is mostly a transcription of the given IARC Mission 7
// iRobot code, available here:
//   http://www.aerialroboticscompetition.org/stories/stories4.php
// ***********************************************************************
#define Meters 1.0f
#define Millimeters 0.001f
#define Seconds 1.0f
#define Robot_Speed (330.0f * Millimeters / Seconds)

// Time between trajectory noise injections
#define Noise_Interval (5.0f * Seconds)

// Time between auto-reverse
#define Reverse_Interval (20.0f * Seconds)

// Time needed to affect trajectory
#define Noise_Length (0.850f * Seconds)

// Time needed to reverse trajectory
// (.33/2 m/s * pi * wheelbase / 2)
#define Reverse_Length (2.456f * Seconds)

// Time needed to spin 45 degrees
#define Top_Touch_Time (Reverse_Length / 4.0f)

#define TransitionTo(state)                \
    {                                      \
    state##Start(event, internal, action); \
    return Robot_##state;                  \
    }

static void
ObstacleWaitStart(robot_Event event,
                  robot_Internal *internal,
                  robot_Action *action)
{
    action->red_led = 1;
}

static void
ObstacleRunStart(robot_Event event,
                 robot_Internal *internal,
                 robot_Action *action)
{
    action->red_led = 1;
    action->green_led = 0;
    action->left_wheel = Robot_Speed - 9 * Millimeters / Seconds;
    action->right_wheel = Robot_Speed + 9 * Millimeters / Seconds;
}

static void
ObstacleCollisionStart(robot_Event event,
                       robot_Internal *internal,
                       robot_Action *action)
{
    action->left_wheel = 0.0f;
    action->right_wheel = 0.0f;
    action->red_led = 1;
    action->green_led = 1;
}

static void
TargetWaitStart(robot_Event event,
                robot_Internal *internal,
                robot_Action *action)
{
    action->green_led = 1;
}

static void
TargetRunStart(robot_Event event,
               robot_Internal *internal,
               robot_Action *action)
{
    action->left_wheel = Robot_Speed;
    action->right_wheel = Robot_Speed;
    action->green_led = 1;
    action->red_led = 0;
}

static void
TrajectoryNoiseStart(robot_Event event,
                     robot_Internal *internal,
                     robot_Action *action)
{
    int offset = random_0_64() - 32;
    float offset_mps = (float)offset * Millimeters / Seconds;
    action->left_wheel = Robot_Speed - offset_mps;
    action->right_wheel = Robot_Speed + offset_mps;
    action->red_led = 1;
    internal->begin_noise = event.elapsed_time;
}

static void
ReverseStart(robot_Event event,
             robot_Internal *internal,
             robot_Action *action)
{
    action->left_wheel = Robot_Speed / 2.0f;
    action->right_wheel = -Robot_Speed / 2.0f;
    action->red_led = 1;
    internal->begin_reverse = event.elapsed_time;
}

static void
TargetCollisionStart(robot_Event event,
                     robot_Internal *internal,
                     robot_Action *action)
{
    action->left_wheel = 0.0f;
    action->right_wheel = 0.0f;
}

static void
TopTouchStart(robot_Event event,
              robot_Internal *internal,
              robot_Action *action)
{
    action->left_wheel = Robot_Speed / 2.0f;
    action->right_wheel = -Robot_Speed / 2.0f;
    action->red_led = 1;
    internal->begin_top_touch = event.elapsed_time;
}

static robot_State
robot_fsm(sim_Robot *robot,robot_State state,
          robot_Internal *internal,
          robot_Event event,
          robot_Action *action)
{
    action->was_bumped = 0;
    action->was_top_touched = 0;
    internal->time_since_last_reverse =  event.elapsed_time - internal->last_reverse;
    internal->time_to_next_reverse =  Reverse_Interval - internal->time_since_last_reverse;


    if (!internal->initialized)
    {
        internal->begin_noise = event.elapsed_time;
        internal->begin_reverse = event.elapsed_time;
        internal->begin_top_touch = event.elapsed_time;
        internal->last_noise = event.elapsed_time;
        internal->last_reverse = event.elapsed_time;

        internal->initialized = true;
    }
    switch (state)
    {
        case Robot_Start:
        {
            if (event.target_switch_pin)
            {
                TransitionTo(TargetWait);
            }
            else
            {
                TransitionTo(ObstacleWait);
            }
        } break;

        case Robot_ObstacleWait:
        {
            if (event.is_run_sig)
            {
                TransitionTo(ObstacleRun);
            }
        } break;

        case Robot_ObstacleRun:
        {
            if (event.is_wait_sig)
            {
                action->left_wheel = 0.0f;
                action->right_wheel = 0.0f;
                TransitionTo(ObstacleWait);
            }
            else if (event.is_bumper)
            {
                action->left_wheel = 0.0f;
                action->right_wheel = 0.0f;
                TransitionTo(ObstacleCollision);
            }
        } break;

        case Robot_ObstacleCollision:
        {
            if (event.is_wait_sig)
            {
                TransitionTo(ObstacleWait);
            }
            else if (!event.is_bumper)
            {
                TransitionTo(ObstacleRun);
            }
        } break;

        case Robot_TargetWait:
        {
            if (event.is_run_sig)
            {
                // Reset noise and reverse timers
                internal->last_noise = event.elapsed_time;
                internal->last_reverse = event.elapsed_time;
                TransitionTo(TargetRun);
            }
        } break;

        case Robot_TargetRun:
        {
            robot->plank_angle = robot->q;
            if (event.is_wait_sig)
            {
                TransitionTo(TargetWait);
            }
            else if (event.is_top_touch)
            {
                TransitionTo(TopTouch);
            }
            else if (event.elapsed_time -
                     internal->last_reverse >
                     Reverse_Interval)
            {
                robot->plank_angle = robot->plank_angle - 1*PI;

                internal->last_reverse = event.elapsed_time;
                internal->time_since_last_reverse =  event.elapsed_time - internal->last_reverse;
                internal->time_to_next_reverse =  Reverse_Interval - internal->time_since_last_reverse;

                internal->reverse_count ++;
                TransitionTo(Reverse);
            }
            else if (event.elapsed_time - internal->last_noise > Noise_Interval)
            {
                TransitionTo(TrajectoryNoise);
            }
            else if (event.is_bumper)
            {
                TransitionTo(TargetCollision);
            }
        } break;

        case Robot_TrajectoryNoise:
        {
            if (event.is_wait_sig)
            {
                TransitionTo(TargetWait);
            }
            else if (event.is_top_touch)
            {
                TransitionTo(TopTouch);
            }
            else if (event.elapsed_time - internal->begin_noise > Noise_Length)
            {
                internal->last_noise = event.elapsed_time;
                TransitionTo(TargetRun);
            }
            else if (event.is_bumper)
            {
                TransitionTo(TargetCollision);
            }
        } break;

        case Robot_Reverse:
        {
            if (event.is_wait_sig)
            {
                TransitionTo(TargetWait);
            }
            else if (event.is_top_touch)
            {
                TransitionTo(TopTouch);
            }
            else if (event.elapsed_time - internal->begin_reverse > Reverse_Length)
            {
                TransitionTo(TargetRun);
            }
        } break;

        case Robot_TargetCollision:
        {
            action->was_bumped = 1;
            internal->reverse_count++;
            robot->plank_angle = robot->plank_angle - PI;
            TransitionTo(Reverse);
        } break;

        case Robot_TopTouch:
        {
            action->was_top_touched = 1;
            if (event.is_wait_sig)
            {
                TransitionTo(TargetWait);
            }
            else if (event.elapsed_time - internal->begin_top_touch > Top_Touch_Time)
            {
                TransitionTo(TargetRun);
            }
            else if (event.is_bumper)
            {
                TransitionTo(TargetCollision);
            }
        } break;
    }

    // Remain in current state
    return state;
}

static sim_World_Angle
wrap_angle(sim_World_Angle angle)
{
    while (angle > TWO_PI)
        angle -= TWO_PI;
    while (angle < 0.0f)
        angle += TWO_PI;
    return angle;
}

static void
robot_integrate(sim_Robot *robot, float dt)
{
    float v = 0.5f * (robot->vl + robot->vr);
    float w = (robot->vr - robot->vl) / (robot->L*0.5f);
    robot->x += v * cosf(robot->q) * dt;
    robot->y += v * sinf(robot->q) * dt;
    robot->q += w * dt;

    // Wrap angles
    robot->q = wrap_angle(robot->q);

    robot->forward_x = cosf(robot->q);
    robot->forward_y = sinf(robot->q);
}

static float
vector_length(float dx, float dy)
{
    return sqrtf(dx*dx + dy*dy);
}

static float
compute_drone_view_radius(float height_above_ground)
{
    // Interpolates between 0.5 meters and
    // 3 meters view radius when height goes
    // from 0 to 2.5 meters.
    return tan(Sim_View_Angle)*height_above_ground;
}

sim_State sim_init(unsigned int seed)
{
    sim_State result;
    INTERNAL = &result;
    DRONE = &INTERNAL->drone;
    ROBOTS = INTERNAL->robots;
    TARGETS = INTERNAL->robots;
    OBSTACLES = INTERNAL->robots + Num_Targets;

    INTERNAL->elapsed_time = 0.0f;

    // Use the seed to set the initial state of the xorshift
    {
        // Pick out some pieces of the number
        // TODO: Does this pattern generate bad sequences?
        INTERNAL->seed = seed;
        INTERNAL->xor128_x = seed & 0xaa121212;
        INTERNAL->xor128_y = seed & 0x21aa2121;
        INTERNAL->xor128_z = seed & 0x1212aa12;
        INTERNAL->xor128_w = seed & 0x212a12aa;

        // Seeds must be nonzero
        if (INTERNAL->xor128_x == 0) INTERNAL->xor128_x++;
        if (INTERNAL->xor128_y == 0) INTERNAL->xor128_y++;
        if (INTERNAL->xor128_z == 0) INTERNAL->xor128_z++;
        if (INTERNAL->xor128_w == 0) INTERNAL->xor128_w++;
    }

    DRONE->x = 10.0f;
    DRONE->y = 10.0f;
    DRONE->z = Sim_Average_Flying_Height; // TODO: Dynamics for z when landing
    DRONE->xr = 10.0f;
    DRONE->yr = 10.0f;
    DRONE->v_max = 1.0f;
    DRONE->cmd.type = sim_CommandType_NoCommand;
    DRONE->cmd.x = 0.0f;
    DRONE->cmd.y = 0.0f;
    DRONE->cmd.i = 0;
    DRONE->landing = false;
    DRONE->on_ground = false;
    DRONE->cmd_done = true;
    DRONE->land_timer = 0.0f;

    for (unsigned int i = 0; i < Num_Targets; i++)
    {
        sim_Robot robot = {};

        // TODO: Verify the distance between the wheels
        // I've read on iRobot's store that the distance
        // is about 0.34 meters. But that, combined with
        // the +-9mm/s wheel speeds does _not_ make the
        // obstacles rotate about the floor center.
        // So something is wrong, either with my dynamics
        // equations, the measurements, or how I'm interpreting
        // the +-9mm/s number. (Does it actually refer to
        // angular velocity?).

        robot.L = Sim_Robot_Wheel_Distance;

        // Spawn each ground robot in a circle

        float t = TWO_PI * i / (float)(Num_Targets);
        //float t = TWO_PI * (_xor128() % 11) / (float)(10);
        robot.x = 10.0f + Sim_Target_Init_Radius * cosf(t);
        robot.y = 10.0f + Sim_Target_Init_Radius * sinf(t);
        robot.q = t;
        //TODO: Spawn each ground robot randomly

        //robot.x = _xor128() % 21;
        //robot.y = _xor128() % 21;
        //robot.q = t;

        robot.internal.initialized = false;
        robot.state = Robot_Start;
        robot.removed = false;

        TARGETS[i] = robot;
    }

    for (unsigned int i = 0; i < Num_Obstacles; i++)
    {
        float t = TWO_PI * i / (float)(Num_Obstacles);

        sim_Robot robot = {};

        robot.L = Sim_Robot_Wheel_Distance;

        // The obstacles are also spawned in a circle,
        // but at an initial radius of 5 meters.
        robot.x = 10.0f + Sim_Obstacle_Init_Radius * cosf(t);
        robot.y = 10.0f + Sim_Obstacle_Init_Radius * sinf(t);
        robot.q = t + PI / 2.0f;
        robot.internal.initialized = false;
        robot.state = Robot_Start;
        robot.removed = false;
        robot.reward = 0;

        OBSTACLES[i] = robot;
    }

    INTERNAL = 0;
    DRONE = 0;
    ROBOTS = 0;
    TARGETS = 0;
    OBSTACLES = 0;

    return result;
}

#include <stdio.h>

sim_State sim_tick(sim_State state, sim_Command new_cmd)
{
    sim_State result = state;
    INTERNAL = &result;
    DRONE = &INTERNAL->drone;
    ROBOTS = INTERNAL->robots;
    TARGETS = INTERNAL->robots;
    OBSTACLES = INTERNAL->robots + Num_Targets;

    INTERNAL->elapsed_time += Sim_Timestep;

    robot_Event events[Num_Robots] = {0};
    for (unsigned int i = 0; i < Num_Robots; i++)
    {
        if (ROBOTS[i].removed) continue;

        events[i].is_run_sig = 0;
        events[i].is_wait_sig = 0;
        events[i].is_top_touch = 0;
        events[i].is_bumper = 0;
        events[i].target_switch_pin = 0;
        events[i].elapsed_time = INTERNAL->elapsed_time;
    }

    if (new_cmd.type != sim_CommandType_NoCommand)
    {
        DRONE->cmd_done = false;
        DRONE->cmd = new_cmd;
    }

    //manage on ground or not
    if(DRONE->z < Sim_Robot_Height )
    {
        DRONE->on_ground = true;
    }else{
        DRONE->on_ground = false;
    }

    switch (DRONE->cmd.type)
    {
        case sim_CommandType_NoCommand:
        {
            // Keep current command
        } break;

        case sim_CommandType_Debug:
        {
            // Keep current command
        } break;

        case sim_CommandType_LandOnTopOf:
        {
            if(DRONE->z < Sim_Average_Flying_Height && ! DRONE->landing )

            {
                DRONE->z += (Sim_Average_Flying_Height/Sim_Take_Off_Time)*Sim_Timestep;
            }
            else
            {
                DRONE->xr = TARGETS[DRONE->cmd.i].x;
                DRONE->yr = TARGETS[DRONE->cmd.i].y;
                float dx = DRONE->xr - DRONE->x;
                float dy = DRONE->yr - DRONE->y;
                float len = sqrtf(dx*dx + dy*dy);
                if (len < Sim_Drone_Target_Proximity)
                {
                    if (!DRONE->landing)
                    {
                        DRONE->landing = true;
                        DRONE->land_timer = Sim_LandOnTopOf_Time;
                    }
                }
                else
                {
                    float v = DRONE->v_max / len;
                    float vx = v * dx;
                    float vy = v * dy;
                    DRONE->x += vx * Sim_Timestep;
                    DRONE->y += vy * Sim_Timestep;
                }
                if (DRONE->landing)
                {
                    DRONE->land_timer -= Sim_Timestep;
                    if (DRONE->land_timer < 0.0f)
                    {
                        events[DRONE->cmd.i].is_top_touch = true;
                        DRONE->land_timer = 0.0f;
                        DRONE->landing = false;
                        DRONE->cmd_done = true;
                        DRONE->cmd.type = sim_CommandType_NoCommand;
                    }
                }
                // if (TARGETS[DRONE->cmd.i].action.was_top_touched)
                // {
                //     DRONE->land_timer = 0.0f;
                //     DRONE->landing = false;
                //     DRONE->cmd_done = true;
                //     DRONE->cmd.type = sim_CommandType_NoCommand;
                // }
            }


        } break;

        case sim_CommandType_LandInFrontOf:
        {
            if(DRONE->z < Sim_Average_Flying_Height && ! DRONE->landing )
            {
                DRONE->z += (Sim_Average_Flying_Height/Sim_Take_Off_Time)*Sim_Timestep;
            }
            else
            {
                DRONE->xr = TARGETS[DRONE->cmd.i].x;
                DRONE->yr = TARGETS[DRONE->cmd.i].y;
                float dx = DRONE->xr - DRONE->x;
                float dy = DRONE->yr - DRONE->y;
                float len = sqrtf(dx*dx + dy*dy);
                if (len < Sim_Drone_Target_Proximity and ROBOTS[DRONE->cmd.i].state !=  Robot_Reverse)
                {
                    if (!DRONE->landing)
                    {
                        DRONE->landing = true;
                        DRONE->land_timer = Sim_LandInFrontOf_Time;
                    }
                }
                else
                {
                    float v = DRONE->v_max / len;
                    float vx = v * dx;
                    float vy = v * dy;
                    DRONE->x += vx * Sim_Timestep;
                    DRONE->y += vy * Sim_Timestep;
                }
                if (DRONE->landing)
                {
                    DRONE->land_timer -= Sim_Timestep;
                    if (DRONE->land_timer < 0.0f)
                    {
                        events[DRONE->cmd.i].is_bumper = true;
                        DRONE->landing = false;
                        DRONE->cmd_done = true;
                        DRONE->cmd.type = sim_CommandType_NoCommand;
                    }
                }
                // if (TARGETS[DRONE->cmd.i].action.was_bumped)
                // {
                //     DRONE->landing = false;
                //     DRONE->cmd_done = true;
                //     DRONE->cmd.type = sim_CommandType_NoCommand;
                // }
          }
        } break;

        case sim_CommandType_Track:
        {
            if(DRONE->z < Sim_Average_Flying_Height && ! DRONE->landing )
            {
                DRONE->z += (Sim_Average_Flying_Height/Sim_Take_Off_Time)*Sim_Timestep;
            }
            else{
                DRONE->xr = TARGETS[DRONE->cmd.i].x;
                DRONE->yr = TARGETS[DRONE->cmd.i].y;
                float dx = DRONE->xr - DRONE->x;
                float dy = DRONE->yr - DRONE->y;
                float len = sqrtf(dx*dx + dy*dy);
                float v = DRONE->v_max / len;
                float vx = v * dx;
                float vy = v * dy;
                DRONE->x += vx * Sim_Timestep;
                DRONE->y += vy * Sim_Timestep;
            }

        } break;

        case sim_CommandType_Search:
        {

            if(DRONE->z < Sim_Average_Flying_Height && ! DRONE->landing )
            {
                DRONE->z += (Sim_Average_Flying_Height/Sim_Take_Off_Time)*Sim_Timestep;
            }
            else{
                DRONE->xr = DRONE->cmd.x;
                DRONE->yr = DRONE->cmd.y;
                float dx = DRONE->xr - DRONE->x;
                float dy = DRONE->yr - DRONE->y;
                float len = sqrtf(dx*dx + dy*dy);

                if (len < Sim_Drone_Goto_Proximity)
                {
                    DRONE->cmd.type = sim_CommandType_NoCommand;
                    DRONE->cmd_done = true;
                }
                else
                {
                    float v = DRONE->v_max / len;
                    float vx = v * dx;
                    float vy = v * dy;
                    DRONE->x += vx * Sim_Timestep;
                    DRONE->y += vy * Sim_Timestep;
                }
            }

        } break;

        case sim_CommandType_TakeOff:
        {
            if(DRONE->z < Sim_Average_Flying_Height)
            {
                // std::cout << "increasing z" << std::endl; 
                DRONE->z += (Sim_Average_Flying_Height/Sim_Take_Off_Time)*Sim_Timestep;
            }
            else{
                // std::cout << "above flying height" << std::endl; 
                DRONE->cmd.type = sim_CommandType_NoCommand;
                DRONE->cmd_done = true;
            }

        } break;

        case sim_CommandType_Land:
        {
            DRONE->xr = DRONE->cmd.x;
            DRONE->yr = DRONE->cmd.y;
            float dx = DRONE->xr - DRONE->x;
            float dy = DRONE->yr - DRONE->y;
            float len = sqrtf(dx*dx + dy*dy);

            if(DRONE->z < Sim_Average_Flying_Height && ! DRONE->landing && len >  Sim_Drone_Target_Proximity){
                    DRONE->z += (Sim_Average_Flying_Height/Sim_Take_Off_Time)*Sim_Timestep;
            }
            else
            {
                if (len <= Sim_Drone_Target_Proximity)
                {
                    if (!DRONE->landing)
                    {
                        DRONE->landing = true;
                    }
                }
                else
                {
                    float v = DRONE->v_max / len;
                    float vx = v * dx;
                    float vy = v * dy;
                    DRONE->x += vx * Sim_Timestep;
                    DRONE->y += vy * Sim_Timestep;
                }

                if (DRONE->z <= 0.0f)
                {

                    DRONE->landing = false;
                    DRONE->cmd_done = true;
                    DRONE->cmd.type = sim_CommandType_NoCommand;
                }
                if (DRONE->landing)
                {
                    DRONE->z -= (Sim_Average_Flying_Height/Sim_Landing_Time)*Sim_Timestep;
                }
            }

        } break;
    }

    struct collision_info
    {
        int hits;
        int bumper_hits;
        float resolve_delta_x;
        float resolve_delta_y;
    };

    collision_info collision[Num_Robots] = {0};

    for (unsigned int i = 0; i < Num_Robots; i++)
    {
        if (ROBOTS[i].removed) continue;

        collision[i].hits = 0;
        collision[i].bumper_hits = 0;
        collision[i].resolve_delta_x = 0.0f;
        collision[i].resolve_delta_y = 0.0f;

        switch (ROBOTS[i].state)
        {
            case Robot_Start:
            {
                if (i < Num_Targets)
                    events[i].target_switch_pin = 1;
                else
                    events[i].target_switch_pin = 0;
            } break;

            case Robot_TargetWait:
            case Robot_ObstacleWait:
            {
                events[i].is_run_sig = 1;
            } break;
        }




        // Check for collisions and compute the average resolve
        // delta vector. The resolve delta will be used to move
        // the robot away so it no longer collides.
        for (unsigned int n = 0; n < Num_Robots; n++)
        {
            if (i == n || ROBOTS[n].removed)
            {
                continue;
            }
            else
            {
                float x1 = ROBOTS[i].x;
                float y1 = ROBOTS[i].y;
                float r1 = Sim_Robot_Radius;
                float x2 = ROBOTS[n].x;
                float y2 = ROBOTS[n].y;
                float r2 = Sim_Robot_Radius;
                float dx = x1 - x2;
                float dy = y1 - y2;
                float L = vector_length(dx, dy);
                float intersection = r2 + r1 - L;
                if (intersection > 0.0f)
                {
                    collision[i].hits++;
                    collision[i].resolve_delta_x += (dx / L) * intersection;
                    collision[i].resolve_delta_y += (dy / L) * intersection;

                    // The robot only reacts (in a fsm sense) if the collision
                    // triggers the bumper sensor in front of the robot. (We
                    // still resolve physical collisions anyway, though).
                    // TODO: Determine the angular region that the bumper
                    // sensor covers (I have assumed 180 degrees).
                    bool on_bumper = (dx * ROBOTS[i].forward_x +
                                      dy * ROBOTS[i].forward_y) <= 0.0f;
                    if (on_bumper)
                        collision[i].bumper_hits++;
                }
            }
        }

        // Check for collisions between Robot and DRONE
        // Compute the average resolve
        // delta vector. The resolve delta will be used to move
        // the robot away so it no longer collides.
       if (DRONE->on_ground){
            float x1 = ROBOTS[i].x;
            float y1 = ROBOTS[i].y;
            float r1 = Sim_Robot_Radius;
            float x2 = DRONE->x;
            float y2 = DRONE->y;
            float r2 = Sim_Drone_Radius;
            float dx = x1 - x2;
            float dy = y1 - y2;
            float L = vector_length(dx, dy);
            float intersection = r2 + r1 - L;
            if (intersection > 0.0f)
            {
                collision[i].hits++;
                collision[i].resolve_delta_x += (dx / L) * intersection;
                collision[i].resolve_delta_y += (dy / L) * intersection;

                // The robot only reacts (in a fsm sense) if the collision
                // triggers the bumper sensor in front of the robot. (We
                // still resolve physical collisions anyway, though).
                // TODO: Determine the angular region that the bumper
                // sensor covers (I have assumed 180 degrees).
                bool on_bumper = (dx * ROBOTS[i].forward_x +
                                  dy * ROBOTS[i].forward_y) <= 0.0f;
                if (on_bumper)
                    collision[i].bumper_hits++;
            }
        }






        if (collision[i].hits > 0)
        {
            collision[i].resolve_delta_x /= (float)collision[i].hits;
            collision[i].resolve_delta_y /= (float)collision[i].hits;
        }
        if (collision[i].bumper_hits > 0)
            events[i].is_bumper = 1;
    }

    for (unsigned int i = 0; i < Num_Robots; i++)
    {
        if (ROBOTS[i].removed)
            continue;
        robot_integrate(&ROBOTS[i], Sim_Timestep);
        if (ROBOTS[i].x < -Sim_Target_Removal_Margin ||
            ROBOTS[i].x > 20.0f + Sim_Target_Removal_Margin ||
            ROBOTS[i].y < -Sim_Target_Removal_Margin)
        {
            ROBOTS[i].removed = true;
            ROBOTS[i].reward--;
        }
        else if(ROBOTS[i].y > 20.0f + Sim_Target_Removal_Margin){
            ROBOTS[i].removed = true;
            ROBOTS[i].reward++;
        }
        if (collision[i].hits > 0)
        {
            ROBOTS[i].x += collision[i].resolve_delta_x * 1.02f;
            ROBOTS[i].y += collision[i].resolve_delta_y * 1.02f;
        }
        ROBOTS[i].state = robot_fsm(&ROBOTS[i],ROBOTS[i].state,
                                    &ROBOTS[i].internal,
                                    events[i],
                                    &ROBOTS[i].action);
        ROBOTS[i].vl = ROBOTS[i].action.left_wheel;
        ROBOTS[i].vr = ROBOTS[i].action.right_wheel;
    }

    INTERNAL = 0;
    DRONE = 0;
    ROBOTS = 0;
    TARGETS = 0;
    OBSTACLES = 0;
    return result;
}

sim_Observed_State sim_observe_state(sim_State state)
{
    sim_Observed_State result = {};
    result.elapsed_time = state.elapsed_time;
    result.drone_x = state.drone.x;
    result.drone_y = state.drone.y;
    result.drone_z = state.drone.z;
    result.drone_cmd_done = state.drone.cmd_done;
    result.num_Targets = Num_Targets;
    sim_Robot *targets = state.robots;
    sim_Robot *obstacles = state.robots + Num_Targets;
    float visible_radius = 2*compute_drone_view_radius(state.drone.z);
    for (unsigned int i = 0; i < Num_Targets; i++)
    {
        float dx = state.drone.x - targets[i].x;
        float dy = state.drone.y - targets[i].y;
        if (vector_length(dx, dy) <= visible_radius)
            result.target_in_view[i] = true;
        else
            result.target_in_view[i] = false;
        result.target_removed[i] = targets[i].removed;
        result.target_reward[i] = targets[i].reward;
        result.target_x[i] = targets[i].x;
        result.target_y[i] = targets[i].y;
        result.target_q[i] = targets[i].q;
        result.target_reversing[i] = (targets[i].state == Robot_Reverse);

    }
    for (unsigned int i = 0; i < Num_Obstacles; i++)
    {
        result.obstacle_x[i] = obstacles[i].x;
        result.obstacle_y[i] = obstacles[i].y;
        result.obstacle_q[i] = obstacles[i].q;
    }

    return result;
}

sim_Observed_State sim_observe_everything(sim_State state)
{
    sim_Observed_State result = {};
    result.elapsed_time = state.elapsed_time;
    result.drone_x = state.drone.x;
    result.drone_y = state.drone.y;
    result.drone_z = state.drone.z;
    result.drone_cmd_done = state.drone.cmd_done;
    sim_Robot *targets = state.robots;
    sim_Robot *obstacles = state.robots + Num_Targets;
    float visible_radius = compute_drone_view_radius(state.drone.z);
    for (unsigned int i = 0; i < Num_Targets; i++)
    {
        float dx = state.drone.x - targets[i].x;
        float dy = state.drone.y - targets[i].y;
        result.target_in_view[i] = true;
        result.target_removed[i] = targets[i].removed;
        result.target_reward[i] = targets[i].reward;
        result.target_x[i] = targets[i].x;
        result.target_y[i] = targets[i].y;
        result.target_q[i] = targets[i].q;
        result.target_reversing[i] = (targets[i].state == Robot_Reverse);

    }
    for (unsigned int i = 0; i < Num_Obstacles; i++)
    {
        result.obstacle_x[i] = obstacles[i].x;
        result.obstacle_y[i] = obstacles[i].y;
        result.obstacle_q[i] = obstacles[i].q;
    }

    return result;
}

static FILE *stbi__fopen(char const *filename, char const *mode)
{
   FILE *f;
#if defined(_MSC_VER) && _MSC_VER >= 1400
   if (0 != fopen_s(&f, filename, mode))
      f=0;
#else
   f = fopen(filename, mode);
#endif
   return f;
}

sim_Observed_State sim_load_snapshot(char *filename)
{
    FILE *file = stbi__fopen(filename, "rb+");
    assert(file);

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char *buffer = (char*)malloc(size);
    assert(buffer);

    size_t read_bytes = fread(buffer, 1, size, file);
    assert(read_bytes == sizeof(sim_Observed_State));

    sim_Observed_State result = *(sim_Observed_State*)buffer;
    fclose(file);
    free(buffer);
    return result;
}

void sim_write_snapshot(char *filename, sim_Observed_State state)
{
    FILE *file = stbi__fopen(filename, "wb+");
    assert(file);
    fwrite((char*)&state, 1, sizeof(sim_Observed_State), file);
    fclose(file);
}


#endif // SIM_IMPLEMENTATION
