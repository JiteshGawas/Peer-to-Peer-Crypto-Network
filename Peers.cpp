#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
#include <set>
#include <map>
#include <sstream>
#include <string>

Peers::Peers(int numNodes, DiscreteEventSimulator &Simulator)
{
    this->numNodes = numNodes;
    srand(time(0));

    PeerVec.resize(Simulator.numNodes);
    for (int i = 0; i < Simulator.numNodes; i++)
    {
        PeerVec[i].NodeId = i;
        PeerVec[i].balance = 30 + rand() % (21); // add some random numbers of BTC between 30-50 for each peer
        PeerVec[i].NWspeed = 1;
        PeerVec[i].CPU_Usage = 1;
        PeerVec[i].Blockchain.insert({0, new Block(1, 0)}); // Genesis Block
        // PeerVec[i].GenerateTransaction(Simulator, "Inits");
    }

    // Randomly Setting z0 % Nodes to have slowNWSpeed
    while (z0_Set.size() < (Simulator.z_0 * Simulator.numNodes))
    {
        z0_Set.insert(rand() % Simulator.numNodes); // cout<<"Z0 peers are"<<endl;
    }
    for (auto it = z0_Set.begin(); it != z0_Set.end(); ++it)
    {
        PeerVec[*it].NWspeed = 0;
    }

    // Randomly Setting z1 % Nodes to be of low_cpu
    while (z1_Set.size() < (Simulator.z_1 * Simulator.numNodes))
        z1_Set.insert(rand() % Simulator.numNodes);
    for (auto it = z1_Set.begin(); it != z1_Set.end(); ++it)
        PeerVec[*it].CPU_Usage = 0;

    // Distributing Hashing Powes Between Peers Such That "High_Cpu" Nodes Have 10 times more hashing power than "Low_Cpu" Nodes
    this->slow_HashPower = 1 / ((this->numNodes * Simulator.z_1) + (this->numNodes - (this->numNodes * Simulator.z_1)) * 10);

    for (int i = 0; i < Simulator.numNodes; i++)
    {
        if (this->PeerVec[i].CPU_Usage == 0) // i.e It is a low_CPU Node
            this->PeerVec[i].hashing_power = this->slow_HashPower;

        else
            this->PeerVec[i].hashing_power = this->slow_HashPower * 10;
    }
}

void Peers::PeerInfo()
{
    cout << "PeerId  Balance  NWSpeed  CPU_Usage Hashing Power" << endl;
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
        cout << PeerVec[i].NodeId << "\t " << PeerVec[i].balance << "\t  " << PeerVec[i].NWspeed << "\t  " << PeerVec[i].CPU_Usage << "\t " << PeerVec[i].hashing_power << endl;
    }

    // To Check Sum Of Hashing Powers Of All The Nodes = 1
    float sum = 0;
    for (int i = 0; i < numNodes; i++)
    {
        sum += this->PeerVec[i].hashing_power;
    }
    if (sum == 1)
        cout << "Hashing Power Distributed Correctly" << endl;
}

void Peers ::setConnectedPeers(Graph &adjMatrix)
{
    for (int i = 0; i < this->numNodes; i++)
    {
        for (int j = 0; j < this->numNodes; j++)
        {
            if (adjMatrix.adjMatrix[i][j] == 1 && i != j)
            {
                this->PeerVec[i].connectedPeers.push_back(&PeerVec[j]);
            }
        }
    }

    // for (int i = 0; i < this->numNodes; i++)
    // {
    //     cout << i << " -> ";
    //     for (int j = 0; j < this->PeerVec[i].connectedPeers.size(); j++)
    //     {

    //         cout << " " << this->PeerVec[i].connectedPeers[j]->NodeId;
    //     }
    //     cout << endl;
    // }
    // cout << endl;
}

float Node ::RandomInterArrivalTxnTime()
{
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    exponential_distribution<float> InterArrivalMeanTime(0.02); // 1/20 i.e exp dist with mean as 20 ms

    return InterArrivalMeanTime(generator);
}

void Node ::GenerateTransaction(DiscreteEventSimulator *Simulator, string TxnType)
{
    // srand(time(0));
    int coins = 5 + rand() % 30;
    int receiverID = rand() % Simulator->numNodes;

    while (this->NodeId == receiverID)
        receiverID = rand() % Simulator->numNodes;

    stringstream ss;

    // A Pays B 60 BTC
    if (TxnType == "Pays")
    {
        ss << this->NodeId << " " << TxnType << " " << receiverID << " " << coins << " BTC";
    }

    // A Mines 60 BTC or A Inits 60 BTC
    if (TxnType == "Mines" || TxnType == "Inits")
    {
        receiverID = this->NodeId;
        ss << this->NodeId << " " << TxnType << " "
           << " " << coins << " BTC"; // A Inits
    }

    string Message = ss.str();

    // cout << Message << endl;
    Transaction *T;
    // cout << T << endl;
    if (TxnType == "Pays")
    {
        if (Simulator->transaction_Counter == 0)
        {
            // cout << "Initial Transaction" << endl;
            T = new Transaction(Message, 0);
        }
        else
        {
            // cout << "Transaction From Inside While" << endl;
            T = new Transaction(Message, Simulator->globalTime + RandomInterArrivalTxnTime());
        }

        this->AllTransactions[T->txnId] = T;

        for (int i = 0; i < this->connectedPeers.size(); i++)
        {
            Simulator->EventQueue.push(new Event(T, "txn_Receive", this, this->connectedPeers[i], Simulator->prop_delay));
        }

        delete T;

        // std::map<std::string, Transaction>::iterator it = this->AllTransactions.begin();

        // while (it != this->AllTransactions.end())
        // {
        //     std::cout << "txnID: " << it->first << ", TxnObject: " << it->second << std::endl;
        //     ++it;
        // }
        if (Simulator->transaction_Counter != 0)
            Simulator->transaction_Counter++;
        // cout << Simulator->transaction_Counter << endl;

        Simulator->EventQueue.push(new Event(this, Simulator->globalTime + RandomInterArrivalTxnTime(), "txn_Generate"));
    }
}

void Node ::ReceiveTransaction(DiscreteEventSimulator *Simulator, Event *currEvent)
{
    if (this->AllTransactions.find(currEvent->T.txnId) == this->AllTransactions.end()) // transactions not in the allTransaction pool
    {
        this->AllTransactions[currEvent->T.txnId] = currEvent->T;

        for (int i = 0; i < this->connectedPeers.size(); i++) // send to connected peers except the receiver
        {
            if (this->connectedPeers[i]->NodeId == currEvent->senderId) // For ensuring loop-less forwarding
                continue;

            Simulator->EventQueue.push(new Event(currEvent->T, "txn_Receive", this, this->connectedPeers[i], Simulator->prop_delay));
        }
    }
}