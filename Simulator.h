#pragma once
#include <iostream>
#include <queue>
#include "Graph.h"
using namespace std;

class Event
{
public:
    float timeStamp;
    int type;
    Event(float t, int ty);
};

class compareTimestamp
{
public:
    bool operator()(const Event &E1, const Event &E2);
};

class DiscreteEventSimulator
{
public:
    int numNodes;
    float z_0, z_1, prop_delay;
    float globalTime = 0;
    int interArrivalTime;
    priority_queue<Event, vector<Event>, compareTimestamp> EventQueue;
    DiscreteEventSimulator(int a, float b, float c, float d);
    void PrintParameters();
    void startSimulation(Graph &adjMatrix);
    friend class Peers;
};
