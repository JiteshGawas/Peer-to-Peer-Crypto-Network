#pragma once
#include <iostream>
#include <queue>
#include "Graph.h"
#include "transactions.h"
#include "Event.h"
using namespace std;

// class Event
// {
// public:
//     float eventTime;
//     int type;
//     int senderId, receiverId;
//     Transaction *T;
//     // Block *B;

//     Event(float t, int ty);
//     Event(Transaction &T);
// };

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
    int interArrivalTime, terminationTime;
    int transactionID_Counter = 0;
    priority_queue<Event, vector<Event>, compareTimestamp> EventQueue;

    DiscreteEventSimulator(int a, float b, float c, float d);
    void PrintParameters();
    void startSimulation(Graph &adjMatrix);
    friend class Peers;
};
