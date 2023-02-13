#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
using namespace std;

#define MAX_Transactions 5000

DiscreteEventSimulator ::DiscreteEventSimulator(int a, float b, float c)
{
    numNodes = a;
    z_0 = b;
    z_1 = c;

    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    uniform_real_distribution<double> latency_distribution(0.01, 0.5); // uniform dist between 10 ms - 500 ms
    prop_delay = latency_distribution(generator);

    this->globalTime = 0;
    this->transaction_Counter = 0;
    this->terminationTime = 60000; // 60000 ms = 60 seconds
    blockInterArrivalMeanTime = 100;
}

void DiscreteEventSimulator ::PrintParameters()
{
    cout << numNodes << " " << z_0 << " " << z_1 << endl;
}

bool compareTimestamp ::operator()(const Event *E1, const Event *E2)
{
    if (E1->eventTime != E2->eventTime)
        return E1->eventTime > E2->eventTime;
    else
        return false;
}

// added comment

void DiscreteEventSimulator ::startSimulation(Graph &adjMatrix, Peers &PeerNetwork)
{
    srand(time(0));
    adjMatrix.createGraph();
    while (!adjMatrix.isConnected())
    {
        cout << "Recreating : Still Not Connected" << endl;
        adjMatrix.createGraph();
    }

    PeerNetwork.setConnectedPeers(adjMatrix);

    for (int i = 0; i < this->numNodes; i++) // Initial Transactions & createBlock Event For All Nodes
    {
        PeerNetwork.PeerVec[i].GenerateTransaction(this, "Pays");
        this->EventQueue.push(new Event(i, 50.0 + PeerNetwork.PeerVec[i].RandomInterArrivalBlockTime(blockInterArrivalMeanTime), "createBlock"));
    }

    this->transaction_Counter = this->numNodes;
    int i = 1;
    // while (!this->EventQueue.empty())
    // {
    //     cout << this->EventQueue.top()->type << endl;
    //     this->EventQueue.pop();
    // }
    while (!this->EventQueue.empty()) //
    {
        currEvent = this->EventQueue.top();
        this->EventQueue.pop();
        this->globalTime = currEvent->eventTime;

        // if (i++ > 500)
        // {
        //     cout << "Transaction Id : " << currEvent->T.txnId << " Event Sender Receiver : " << currEvent->senderId << "->" << currEvent->receiverId << " Event Type : " << currEvent->type << " Event Time : " << currEvent->eventTime << endl;
        //     continue;
        // }

        if (currEvent->type == "txn_Generate") // To Generate Transactions after every interArrivalTxnTime Step
        {
            PeerNetwork.PeerVec[currEvent->senderId].GenerateTransaction(this, "Pays");
            // cout << "Event Sender : " << currEvent->senderId << " Event Receiver : " << currEvent->receiverId << " Event Type : " << currEvent->type << " Event Time : " << currEvent->eventTime << endl;
        }

        if (currEvent->type == "txn_Receive")
        {
            // cout << "InReceive" << endl;
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveTransaction(this, currEvent);
            // break;
            // cout << "Event Sender : " << currEvent->senderId << " Event Receiver : " << currEvent->receiverId << " Event Type : " << currEvent->type << " Event Time : " << currEvent->eventTime << " " << currEvent->T << endl;
        }

        if (currEvent->type == "createBlock")
        {
            cout << "createBlock : Block Created By " << currEvent->senderId << endl;
            PeerNetwork.PeerVec[currEvent->senderId].GenerateBlock(this, &(PeerNetwork.BlockCounter));
            // break;
            // cout << "Event Sender : " << currEvent->senderId << " Event Receiver : " << currEvent->receiverId << " Event Type : " << currEvent->type << " Event Time : " << currEvent->eventTime << " " << currEvent->T << endl;
        }

        if (currEvent->type == "MineBlock")
        {
            cout << "MineBlock : Block Mined By " << currEvent->senderId << endl;
            PeerNetwork.PeerVec[currEvent->senderId].MineBlock(this, currEvent, &(PeerNetwork.BlockCounter));
            // break;
            // cout << "Event Sender : " << currEvent->senderId << " Event Receiver : " << currEvent->receiverId << " Event Type : " << currEvent->type << " Event Time : " << currEvent->eventTime << " " << currEvent->T << endl;
        }
        if (currEvent->type == "ReceiveBlock")
        {
            cout << "ReceiveBlock : Block is Received by " << currEvent->receiverId << endl;
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveBlock(this, currEvent);
            // break;
            // cout << "Event Sender : " << currEvent->senderId << " Event Receiver : " << currEvent->receiverId << " Event Type : " << currEvent->type << " Event Time : " << currEvent->eventTime << " " << currEvent->T << endl;
        }
        // cout << "Transaction Id : " << currEvent->T.txnId << " Event Sender Receiver : " << currEvent->senderId << "->" << currEvent->receiverId << " Event Type : " << currEvent->type << " Event Time : " << currEvent->eventTime << endl;

        delete currEvent;

        // if (i++ == 10000) //
        //     break;

        if (this->terminationTime < this->globalTime)
            break;
        // if (this->transaction_Counter > MAX_Transactions)
        //     break;
    }

    // for (int i = 0; i < this->numNodes; i++)
    // {
    //     cout << "---------------------------------Blockchain At Peer " << i + 1 << "------------------------------------------" << endl;
    //     auto itr = PeerNetwork.PeerVec[i].Blockchain.begin();
    //     while (itr != PeerNetwork.PeerVec[i].Blockchain.end())
    //     {
    //         cout << "BlockId : " << itr->first << " ParentId : " << itr->second.PrevHash << endl;
    //         // cout << "        =======Transactions For The Block==========" << endl;
    //         // for (int i = 0; i < itr->second.Transactions.size(); i++)
    //         // {
    //         //     cout << itr->second.Transactions[i].getMessage() << endl;
    //         // }
    //         /*Printing Node Balances of each Block*/
    //         cout << "        =======Node Balances For The Block==========" << endl;
    //         for (int i = 0; i < itr->second.NodeBalances.size(); i++)
    //         {
    //             cout << itr->second.NodeBalances[i] << " ";
    //         }
    //         cout << endl;
    //         itr++;
    //     }

    //     cout << "BlockChain Length of Peer " << i + 1 << " : " << PeerNetwork.PeerVec[i].blockChainLength << endl;
    // }
    // while (!this->EventQueue.empty())
    // {
    //     cout << this->EventQueue.top()->type << endl;
    //     this->EventQueue.pop();
    // }
}
