#include <iostream>
#include <random>
#include <chrono>
#include "Simulator.h"
#include "Graph.h"
using namespace std;

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
}

void DiscreteEventSimulator ::PrintParameters()
{
    cout << numNodes << " " << z_0 << " " << z_1 << " " << interArrivalTime << endl;
}

Event ::Event(float t, int ty)
{
    timeStamp = t;
    type = ty;
}

bool compareTimestamp ::operator()(const Event &E1, const Event &E2)
{
    if (E1.timeStamp > E2.timeStamp)
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
    for (int i = 0; i < adjMatrix.adjMatrix.size(); i++)
    {
        for (int j = 0; j < adjMatrix.adjMatrix[i].size(); j++)
        {
            cout << adjMatrix.adjMatrix[i][j] << " ";
        }
        cout << endl;
    }
}