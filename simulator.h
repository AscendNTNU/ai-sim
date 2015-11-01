#ifndef _simulator_h_
#define _simulator_h_
#ifdef SIM_CLIENT_CODE
#define RECV_PORT 22334
#define SEND_PORT 22335
#else
#define RECV_PORT 22335
#define SEND_PORT 22334
#endif

// The simulator uses UDP packets for interprocess
// communication. Internally, the simulator polls
// for UDP packets containing a high-level command
// that the drone will execute. Furthermore, the
// simulator sends the complete world state at a
// predefined rate.

// To get state from the simulator and send drone
// commands to the simulator, do the following:
// #define SIM_CLIENT_CODE in your application,
// and include this file.

// The functions
//  sim_init_msgs
//  sim_recv_state
//  sim_send_cmd
// can be used to communicate with the simulator.

// For example usage, see example_ai.cpp.

#define Num_Targets 10
#define Num_Obstacles 4
struct Robot
{
    float x; // x-coordinate in world coordinates
    float y; // y-coordinate in world coordinates
    float q; // rotation relative world x-axis
    float vl; // left wheel speed
    float vr; // right wheel speed
};

struct Drone
{
    float x;
    float y;
    float z;
};

struct SimulationState
{
    float elapsed_sim_time;
    Robot targets[Num_Targets];
    Robot obstacles[Num_Obstacles];
    Drone drone;
};

// The drone command interface is still uncertain
// and will most likely change alot over time. So
// for now I provide an example of what it might
// look like.
enum DroneCmdType
{
    // land on the closest ground robot
    DroneCmdType_TouchTop = 0,

    // go to a given position (x, y)
    DroneCmdType_Goto
};

struct DroneCmd
{
    DroneCmdType type;
    float x;
    float y;
};

#define UDP_IMPLEMENTATION
#include "udp.h"

void sim_init_msgs(bool nonblocking)
{
    udp_open(RECV_PORT, nonblocking);
}

bool sim_recv_state(SimulationState *result)
{
    SimulationState state = {};
    uint32_t read_bytes = udp_recv((char*)&state, sizeof(SimulationState), 0);
    if (read_bytes == sizeof(SimulationState))
    {
        *result = state;
        return true;
    }
    else
    {
        return false;
    }
}

void sim_send_cmd(DroneCmd *cmd)
{
    udp_addr dst = { 127, 0, 0, 1, SEND_PORT };
    udp_send((char*)cmd, sizeof(DroneCmd), dst);
}

bool sim_recv_cmd(DroneCmd *result)
{
    DroneCmd cmd = {};
    uint32_t read_bytes = udp_recv((char*)&cmd, sizeof(DroneCmd), 0);
    if (read_bytes == sizeof(DroneCmd))
    {
        *result = cmd;
        return true;
    }
    else
    {
        return false;
    }
}

void sim_send_state(SimulationState *state)
{
    udp_addr dst = { 127, 0, 0, 1, SEND_PORT };
    udp_send((char*)state, sizeof(SimulationState), dst);
}

#endif
