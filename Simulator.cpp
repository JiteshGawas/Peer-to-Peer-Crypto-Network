#include <iostream>
#include <random>
#include <chrono>
#include "Simulator.h"
#include "Graph.h"
#include "Event.h"
using namespace std;

#define MAX_Transactions 500

DiscreteEventSimulator ::DiscreteEventSimulator(int a, float b, float c, float d)
{
    numNodes = a;
    z_0 = b;
    z_1 = c;

    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    exponential_distribution<float> InterArrivalMeanTime(d); // have to take this argument from command line

    interArrivalTime = InterArrivalMeanTime(generator);

    uniform_real_distribution<double> latency_distribution(0.01, 0.5);
    prop_delay = latency_distribution(generator);

    this->terminationTime = 60000; // 60000 ms = 60 seconds
}

void DiscreteEventSimulator ::PrintParameters()
{
    cout << numNodes << " " << z_0 << " " << z_1 << " " << interArrivalTime << endl;
}

bool compareTimestamp ::operator()(const Event &E1, const Event &E2)
{
    if (E1.eventTime > E2.eventTime)
        return true;
    else
        return false;
}

// added comment

void DiscreteEventSimulator ::startSimulation(Graph &adjMatrix)
{
    adjMatrix.createGraph();
    while (!adjMatrix.isConnected())
    {
        cout << "Recreating : Still Not Connected" << endl;
        adjMatrix.createGraph();
    }

    //-- -- -- -- -- -Printing The Adjacency Matrix-- -- -- -- -- -- -- -- -- -- --
    // for (int i = 0; i < adjMatrix.adjMatrix.size(); i++)
    // {
    //     for (int j = 0; j < adjMatrix.adjMatrix[i].size(); j++)
    //     {
    //         cout << adjMatrix.adjMatrix[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    while (!this->EventQueue.empty())
    {
        cout << this->EventQueue.top().T << endl;
        this->EventQueue.pop();
    }

    // while (((this->terminationTime > this->globalTime) && !this->EventQueue.empty()) || this->EventQueue.size() > MAX_Transactions)
    // {
    //     Event currEvent = this->EventQueue.top();
    //     this->EventQueue.pop(); // to remove from the EventQueue
    //     // if(currEvent.type)
    // }
}
