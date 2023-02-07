#include <iostream>
#include <random>
#include "Graph.h"
#include <set>
#include <map>
#include "Simulator.h"
#include "Blockchain.h"
#include "Peers.h"

// class Node
// {
// public:
//     int NodeId, NWspeed, CPU_Usage, chainLength;
//     float balance;
//     map<int, Block *> Blockchain;
//     // NWspeed : 0 (Slow) 1(Fast) | CPU_Usage = 0 (low_cpu) 1(high_cpu)
//     bool isConnected(const Graph &adjMatrix, int peerId)
//     {
//         if (adjMatrix.adjMatrix[this->NodeId][peerId])
//             return true;
//         else
//             return false;
//     }
// };

Peers::Peers(int numNodes, const DiscreteEventSimulator &Simulator)
{
    this->numNodes = numNodes;

    PeerVec.resize(Simulator.numNodes);
    for (int i = 0; i < Simulator.numNodes; i++)
    {
        PeerVec[i].NodeId = i;
        PeerVec[i].balance = i; // add some random numbers of BTC
        PeerVec[i].NWspeed = 1;
        PeerVec[i].CPU_Usage = 1;
        PeerVec[i].Blockchain.insert({0, new Block(1, 0)});
    }

    srand(time(0));
    while (z0_Set.size() < (Simulator.z_0 * Simulator.numNodes))
    {
        z0_Set.insert(rand() % Simulator.numNodes); // cout<<"Z0 peers are"<<endl;
    }
    for (auto it = z0_Set.begin(); it != z0_Set.end(); ++it)
    {
        PeerVec[*it].NWspeed = 0;
    }

    while (z1_Set.size() < (Simulator.z_1 * Simulator.numNodes))
        z1_Set.insert(rand() % Simulator.numNodes);
    for (auto it = z1_Set.begin(); it != z1_Set.end(); ++it)
        PeerVec[*it].CPU_Usage = 0;
}

void Peers::PeerInfo()
{
    cout << "PeerId  Balance  NWSpeed  CPU_Usage" << endl;
    // cout << "BlockID, PrevHash, Blockhash" << endl;
    for (int i = 0; i < numNodes; i++)
    {
        // map<int, Block *>::iterator it = PeerVec[i].Blockchain.begin();

        // Iterate through the map and print the elements
        // while (it != PeerVec[i].Blockchain.end())
        // {
        //     std::cout << "BlockId: " << it->second->blockId << ", PrevHash: " << it->second->PrevHash << ", BlockHash: " << it->second->BlockHash << std::endl;
        //     ++it;
        // }
        cout << PeerVec[i].NodeId << "  " << PeerVec[i].balance << "  " << PeerVec[i].NWspeed << "  " << PeerVec[i].CPU_Usage << endl;
    }
};
