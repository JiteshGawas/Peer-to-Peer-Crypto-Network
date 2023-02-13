#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
using namespace std;

// #define MAX_Transactions 5000

DiscreteEventSimulator ::DiscreteEventSimulator(int a, float b, float c, float d)
{
    numNodes = a;
    z_0 = b;
    z_1 = c;

    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    exponential_distribution<float> InterArrivalMeanTime(d); // have to take this argument from command line

    interArrivalTxnTime = InterArrivalMeanTime(generator);

    uniform_real_distribution<double> latency_distribution(0.01, 0.5);
    prop_delay = latency_distribution(generator);

    this->globalTime = 0;
    this->terminationTime = 60000; // 60000 ms = 60 seconds
}

void DiscreteEventSimulator ::PrintParameters()
{
    cout << numNodes << " " << z_0 << " " << z_1 << " " << interArrivalTxnTime << endl;
}

bool compareTimestamp ::operator()(const Event *E1, const Event *E2)
{
    if (E1->eventTime > E2->eventTime)
        return true;
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

    //-- -- -- -- -- -Printing The Adjacency Matrix-- -- -- -- -- -- -- -- -- -- --
    // for (int i = 0; i < adjMatrix.adjMatrix.size(); i++)
    // {
    //     for (int j = 0; j < adjMatrix.adjMatrix[i].size(); j++)
    //     {
    //         cout << adjMatrix.adjMatrix[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    PeerNetwork.setConnectedPeers(adjMatrix);
    
    // while (!this->EventQueue.empty())
    // {
    //        //     cout << this->EventQueue.top().T << endl;
    //     this->EventQueue.pop();
    // }
    
    int i = 1;
    // cout << "Outside" << endl;
    while (this->terminationTime > this->globalTime)
    {
        // cout << "GenerateDone" << endl;
        PeerNetwork.PeerVec[rand() % this->numNodes].GenerateTransaction(this, "Pays"); // randomly Generating Transactions between interArrival Txn Time

        // if (!(this->EventQueue.size() < MAX_Transactions) || this->EventQueue.empty())
        //     break;
        currEvent = this->EventQueue.top();
        this->EventQueue.pop();

        if (currEvent->type == "txn_Receive")
        {
            // cout << "InReceive" << endl;
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveTransaction(this, currEvent);
            // break;
        }

        if (currEvent->type == "ReceiveBlock")
        {
            // cout << "InReceive" << endl;
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveBlock(this,currEvent);
            // break;
        }

        // cout << "Event Sender : " << currEvent->senderId << " Event Receiver : " << currEvent->receiverId << " " << currEvent->T << endl;
        // cout << currEvent->eventTime << " " << this->globalTime << endl;
        this->globalTime = currEvent->eventTime;

  
        delete currEvent;
        if (i++ == 10000)
            break;
    }

    PeerNetwork.PeerVec[6].GenerateBlock(this);
    // this->globalTime = 0;
    while (this->terminationTime > this->globalTime)
    {
        cout << "DusreWhileMe" << endl;
        PeerNetwork.PeerVec[rand() % this->numNodes].GenerateTransaction(this, "Pays"); // randomly Generating Transactions between interArrival Txn Time

        // if (!(this->EventQueue.size() < MAX_Transactions) || this->EventQueue.empty())
        //     break;
        currEvent = this->EventQueue.top();
        this->EventQueue.pop();

        
        if (currEvent->type == "ReceiveBlock")
        {
            // cout << "InReceive" << endl;
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveBlock(this,currEvent);
            // break;
        }

        // cout << "Event Sender : " << currEvent->senderId << " Event Receiver : " << currEvent->receiverId << " " << currEvent->T << endl;
        // cout << currEvent->eventTime << " " << this->globalTime << endl;
        this->globalTime = currEvent->eventTime;

  
        delete currEvent;
        if (i++ == 20000)
            break;
    }
    
    for(auto itr = PeerNetwork.PeerVec[6].Blockchain.begin(); itr!= PeerNetwork.PeerVec[6].Blockchain.end(); itr++)
    {
        cout<<"Prev Block Id : "<<itr->second.PrevHash<<"  BlockID : "<<itr->first<<endl;
    }

}
