#include <iostream>
#include <chrono>
#include <random>
#include <set>
#include <vector>
#include <time.h>
#include "ClassDeclarations.h"

using namespace std;

#define MAX_PEERS 20

//-------------------------------------------------Main Begins----------------------------------------------------------------------

int main()
{

    DiscreteEventSimulator Simulator(MAX_PEERS, 0.2, 0.4, 0.1);
    Graph adjMatrix(Simulator.numNodes, 4, 8);
    Peers Peers(Simulator.numNodes, Simulator); // object of class having peer having vector of nodes

    Simulator.PrintParameters();
    // Peers.PeerInfo();
    Simulator.startSimulation(adjMatrix, Peers);
}

// Transaction T("27 : 89 Mines 40 69.0 BTC", 0.8);
// cout << T << endl;
// float calculate_delay(Node Sender, Node Receiver, int message_size);
// void PopulateLatencyMatrix(vector<Node> Peers);