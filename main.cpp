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

int main(int argc, char *argv[])
{
    // #peers        zo              z1             advMinPow  : also need to take adversary connected to honest node percentage by command line
    DiscreteEventSimulator Simulator(stoi(argv[1]), stof(argv[2]), stof(argv[3]), stof(argv[4]), 20, 100); // InterArrivalTimeMean {argv[1], argv[2] are z0 and z1}. 20 and 100 are interarrival transaction and block time respectively;
    Graph adjMatrix(Simulator.numNodes, 4, 8);
    Peers Peers(Simulator.numNodes, Simulator, argv[5]); // object of class having peer having vector of nodes

    Simulator.PrintParameters();
    Peers.PeerInfo();

    Simulator.startSimulation(adjMatrix, Peers);
    return 0;
}

// Transaction T("27 : 89 Mines 40 69.0 BTC", 0.8);
// cout << T << endl;
// float calculate_delay(Node Sender, Node Receiver, int message_size);
// void PopulateLatencyMatrix(vector<Node> Peers);