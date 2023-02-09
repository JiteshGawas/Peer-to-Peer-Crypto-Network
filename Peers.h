#pragma once
#include <random>
#include "Graph.h"
#include <set>
#include <map>
#include "Simulator.h"
#include "Blockchain.h"

class Node
{
public:
    int NodeId, NWspeed, CPU_Usage;
    float balance;
    map<int, Block *> Blockchain;
    // NWspeed : 0 (Slow) 1(Fast) | CPU_Usage = 0 (low_cpu) 1(high_cpu)
    bool isConnected(const Graph &adjMatrix, int peerId)
    {
        if (adjMatrix.adjMatrix[this->NodeId][peerId])
            return true;
        else
            return false;
    }
    void GenerateTransaction(DiscreteEventSimulator &Simulator, string TxnType);
};

class Peers
{
public:
    vector<Node> PeerVec; // Vector of PeerVec
    int numNodes;
    set<int> z0_Set, z1_Set;

    Peers(int numNodes, DiscreteEventSimulator &Simulator);
    void PeerInfo();
};
