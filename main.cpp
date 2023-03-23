#include <iostream>
#include <chrono>
#include <random>
#include <set>
#include <vector>
#include <time.h>
#include "ClassDeclarations.h"

using namespace std;

#define MAX_PEERS 21

//-------------------------------------------------Main Begins----------------------------------------------------------------------
// Usage : ./Build/main num_peers z0 z1 advMinPow advConPer advType
int main(int argc, char *argv[])
{
    // #peers        zo              z1             advMinPow  : also need to take adversary connected to honest node percentage by command line

    DiscreteEventSimulator Simulator(stoi(argv[1]), stof(argv[2]), stof(argv[3]), stof(argv[4]), stof(argv[5]), 20, 100, argv[6]); // InterArrivalTimeMean {argv[1], argv[2] are z0 and z1}. 20 and 100 are interarrival transaction and block time respectively;
    Graph adjMatrix(Simulator.numNodes, 4, 8);
    Peers Peers(Simulator.numNodes, Simulator, argv[6]); // object of class having peer having vector of nodes

    Simulator.PrintParameters();
    Peers.PeerInfo();

    Simulator.startSimulation(adjMatrix, Peers);
    return 0;
}
