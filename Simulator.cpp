#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
#include <fstream>
#include <experimental/filesystem>
#include <unistd.h>
using namespace std;

#define MAX_Transactions 5000

DiscreteEventSimulator ::DiscreteEventSimulator(int numPeers, float z0, float z1, float advMinPow, float txnMean, float blkMean)
{
    numNodes = numPeers;
    z_0 = z0;
    z_1 = z1;
    txnInterArrivalMeanTime = txnMean;
    blockInterArrivalMeanTime = blkMean;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    uniform_real_distribution<double> latency_distribution(0.01, 0.5); // uniform dist between 10 ms - 500 ms
    prop_delay = latency_distribution(generator);

    this->globalTime = 0;
    this->transaction_Counter = 0;
    this->terminationTime = 1000; // Termination time

    DateTime = __DATE__ " - " __TIME__;
    this->advMinPow = advMinPow;
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
    while (!adjMatrix.isConnected()) // Keep Creating Graph unless it is connected
    {
        cout << "Recreating : Still Not Connected" << endl;
        adjMatrix.createGraph();
    }

    PeerNetwork.setConnectedPeers(adjMatrix); // Set connected nodes vector for each node

    for (int i = 0; i < this->numNodes; i++) // Initial Transactions & createBlock Event For All Nodes
    {
        PeerNetwork.PeerVec[i].GenerateTransaction(this, "Pays");
        this->EventQueue.push(new Event(i, 50.0 + PeerNetwork.PeerVec[i].RandomInterArrivalBlockTime(blockInterArrivalMeanTime), "createBlock"));
    }

    this->transaction_Counter = this->numNodes;
    int i = 1;

    int eventCounter = 0;

    while (!this->EventQueue.empty()) //
    {
        currEvent = this->EventQueue.top();
        this->EventQueue.pop();
        this->globalTime = currEvent->eventTime;
        eventCounter++;

        if (currEvent->type == "txn_Generate") // To Generate Transactions after every interArrivalTxnTime Step
        {
            PeerNetwork.PeerVec[currEvent->senderId].GenerateTransaction(this, "Pays");
        }

        if (currEvent->type == "txn_Receive") // Receive Transaction
        {
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveTransaction(this, currEvent);
        }

        if (currEvent->type == "createBlock") // Generate Block
        {
            PeerNetwork.PeerVec[currEvent->senderId].GenerateBlock(this, &(PeerNetwork.BlockCounter));
        }

        if (currEvent->type == "MineBlock") // Mine Block
        {
            PeerNetwork.PeerVec[currEvent->senderId].MineBlock(this, currEvent, &(PeerNetwork.BlockCounter));
        }
        if (currEvent->type == "ReceiveBlock") // Receive Block
        {
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveBlock(this, currEvent, &(PeerNetwork.BlockCounter));
        }

        delete currEvent;

        // if (i++ == 10000) //
        //     break;

        if (this->terminationTime < this->globalTime)
            break;
        // if (this->transaction_Counter > MAX_Transactions)
        //     break;
    }

    this->writeBlockArrivalTimes(PeerNetwork, this->DateTime);
    this->writeBlockChain(PeerNetwork, this->DateTime);
    this->writeGraphDetails(PeerNetwork, adjMatrix, this->DateTime);
    this->writeNodeDetails(PeerNetwork, this->DateTime);
    // writePeerInfo
    for (int i = 0; i < this->numNodes; i++)
    {
        // if (i == 1)
        //     break;
        cout << "---------------------------------Blockchain At Peer " << i + 1 << "------------------------------------------" << endl;
        auto itr = PeerNetwork.PeerVec[i].Blockchain.begin();
        while (itr != PeerNetwork.PeerVec[i].Blockchain.end() && PeerNetwork.PeerVec[i].Blockchain.size() != 1)
        {
            cout << "BlockId : " << itr->first << " ParentId : " << itr->second.PrevHash << " Block Mined By : " << itr->second.minedId << endl;

            itr++;
        }
        cout << "BlockChain Length of Peer " << i + 1 << " : " << PeerNetwork.PeerVec[i].blockChainLength << endl;
        cout << "All Blocks Including Forks at Peer " << i + 1 << " : " << PeerNetwork.PeerVec[i].Blockchain.size() << endl;
    }
    cout << "Event Executed : " << eventCounter << endl;
    cout << "Event Pending in Queue : " << this->EventQueue.size() << endl;
}

void DiscreteEventSimulator ::writeBlockArrivalTimes(Peers &PeerNetwork, string DateTime)
{
    namespace fs = std::experimental::filesystem;
    string currentPath = fs::current_path();
    string directoryName = fs::current_path() / "Logs" / DateTime / "BlockArrivalTimes";
    fs::create_directories(directoryName);
    chdir(directoryName.c_str());

    for (int i = 0; i < PeerNetwork.numNodes; i++)
    {
        string arg = "Node-" + to_string(i + 1) + ".txt";
        ofstream BlockArrivalLog(arg);
        for (auto itr = PeerNetwork.PeerVec[i].BlockArrivalTimes.begin(); itr != PeerNetwork.PeerVec[i].BlockArrivalTimes.end(); ++itr)
        {
            BlockArrivalLog << itr->first << " " << itr->second << endl;
        }
        BlockArrivalLog.close();
    }
    chdir(currentPath.c_str());
}

void DiscreteEventSimulator ::writeBlockChain(Peers &PeerNetwork, string DateTime) // Write the Blockchain details to Text files in Logs Folder
{
    namespace fs = std::experimental::filesystem;
    string currentPath = fs::current_path();
    string directoryName = fs::current_path() / "Logs" / DateTime / "BlockChain";
    fs::create_directories(directoryName);
    chdir(directoryName.c_str());

    for (int i = 0; i < PeerNetwork.numNodes; i++)
    {
        string arg = "Node-" + to_string(i + 1) + ".txt";
        ofstream BlockChainLog(arg);
        for (auto itr = PeerNetwork.PeerVec[i].Blockchain.begin(); itr != PeerNetwork.PeerVec[i].Blockchain.end(); ++itr)
        {
            BlockChainLog << itr->first << " " << itr->second.PrevHash << endl;
        }
        BlockChainLog.close();
    }
    chdir(currentPath.c_str());
}

void DiscreteEventSimulator ::writeGraphDetails(Peers &PeerNetwork, Graph &adjMatrix, string DateTime) // Write the graph details to a txt file
{
    namespace fs = std::experimental::filesystem;
    string currentPath = fs::current_path();
    string directoryName = fs::current_path() / "Logs" / DateTime;
    fs::create_directories(directoryName);
    chdir(directoryName.c_str());
    string arg = directoryName + "/GraphDetails.txt";
    ofstream GraphDetailsLog(arg);

    GraphDetailsLog << "[";
    for (int i = 0; i < PeerNetwork.numNodes; i++)
    {
        GraphDetailsLog << "[";
        for (int j = 0; j < PeerNetwork.numNodes; j++)
        {
            GraphDetailsLog << adjMatrix.adjMatrix[i][j];
            if (j == PeerNetwork.numNodes - 1)
                GraphDetailsLog << "]";
            else
                GraphDetailsLog << ",";
        }
        if (i == PeerNetwork.numNodes - 1)
            GraphDetailsLog << "]";
        else
            GraphDetailsLog << ",";
    }
    GraphDetailsLog << endl;
    GraphDetailsLog << "[";
    vector<int> slowNodes;
    for (int i = 0; i < PeerNetwork.numNodes; i++)
    {
        if (PeerNetwork.PeerVec[i].NWspeed == 0)
            slowNodes.push_back(i);
    }
    for (int i = 0; i < slowNodes.size(); i++)
    {
        GraphDetailsLog << slowNodes[i];
        if (i == slowNodes.size() - 1)
            GraphDetailsLog << "]";
        else
            GraphDetailsLog << ",";
    }
    chdir(currentPath.c_str());
}

void DiscreteEventSimulator ::writeNodeDetails(Peers &PeerNetwork, string DateTime) // Write node details like z0,z1, Number of nodes mined to a text file
{
    namespace fs = std::experimental::filesystem;
    string currentPath = fs::current_path();
    string directoryName = fs::current_path() / "Logs" / DateTime;
    fs::create_directories(directoryName);
    chdir(directoryName.c_str());

    float MinedInLongestChain, totalMinedBlocks;
    float ratio;

    string arg = "NodeDetails.csv";
    ofstream NodeDetailsLog(arg);
    NodeDetailsLog << "NodeId,SpeedNode,CpuUsageNode,BlocksMinedInLongestChain,TotalBlocksMined,Ratio" << endl;
    for (int j = 0; j < PeerNetwork.numNodes; j++)
    {
        MinedInLongestChain = PeerNetwork.PeerVec[j].getMinedInLongestChain();
        totalMinedBlocks = PeerNetwork.PeerVec[j].getTotalMinedBlocks();
        ratio = MinedInLongestChain / totalMinedBlocks;
        if (!(MinedInLongestChain && totalMinedBlocks))
            NodeDetailsLog << j + 1 << "," << PeerNetwork.PeerVec[j].NWspeed << "," << PeerNetwork.PeerVec[j].CPU_Usage << "," << MinedInLongestChain << "," << totalMinedBlocks << ",NA" << endl;
        else
            NodeDetailsLog << j + 1 << "," << PeerNetwork.PeerVec[j].NWspeed << "," << PeerNetwork.PeerVec[j].CPU_Usage << "," << MinedInLongestChain << "," << totalMinedBlocks << "," << ratio << endl;
    }
    NodeDetailsLog.close();

    chdir(currentPath.c_str());
}
