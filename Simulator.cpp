#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
#include <fstream>
#include <experimental/filesystem>
#include <unistd.h>
using namespace std;

#define MAX_Transactions 5000

DiscreteEventSimulator ::DiscreteEventSimulator(int numPeers, float z0, float z1, float advMinPow, float advConPer, float txnMean, float blkMean)
{
    numNodes = numPeers;
    z_0 = z0;
    z_1 = z1;
    txnInterArrivalMeanTime = txnMean;
    blockInterArrivalMeanTime = blkMean;
    this->advConPer = advConPer;
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    uniform_real_distribution<double> latency_distribution(0.01, 0.5); // uniform dist between 10 ms - 500 ms
    prop_delay = latency_distribution(generator);

    this->globalTime = 0;
    this->transaction_Counter = 0;
    this->terminationTime = 3000; // Termination time

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

void DiscreteEventSimulator ::startSimulation(Graph &adjMatrix, Peers &PeerNetwork)
{
    srand(time(0));
    adjMatrix.createGraph(this->advConPer, PeerNetwork.numHonest);
    while (!adjMatrix.isConnected()) // Keep Creating Graph unless it is connected
    {
        cout << "Recreating : Still Not Connected" << endl;
        adjMatrix.createGraph(this->advConPer, PeerNetwork.numHonest);
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


        if (this->terminationTime < this->globalTime)
            break;

    }

    this->writeBlockArrivalTimes(PeerNetwork, this->DateTime);
    this->writeBlockChain(PeerNetwork, this->DateTime);
    this->writeGraphDetails(PeerNetwork, adjMatrix, this->DateTime);
    this->writeNodeDetails(PeerNetwork, this->DateTime);
    this->write_Params_Ratios(PeerNetwork, this->DateTime);
    for (int i = 0; i < this->numNodes; i++)
    {

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
            BlockChainLog << itr->first << " " << itr->second.PrevHash << " " << itr->second.minedId << " " << itr->second.minedTime << endl;
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
    NodeDetailsLog << "NodeId,SpeedNode,CpuUsageNode,BlocksMinedInLongestChain,TotalBlocksMined,Ratio,Hashing Power" << endl;
    for (int j = 0; j < PeerNetwork.numNodes; j++)
    {
        MinedInLongestChain = PeerNetwork.PeerVec[j].getMinedInLongestChain();
        totalMinedBlocks = PeerNetwork.PeerVec[j].getTotalMinedBlocks();
        ratio = MinedInLongestChain / totalMinedBlocks;
        if (!(MinedInLongestChain && totalMinedBlocks))
            NodeDetailsLog << j + 1 << "," << PeerNetwork.PeerVec[j].NWspeed << "," << PeerNetwork.PeerVec[j].CPU_Usage << "," << MinedInLongestChain << "," << totalMinedBlocks << ",NA" << "," << PeerNetwork.PeerVec[j].hashing_power <<endl;
        else
            NodeDetailsLog << j + 1 << "," << PeerNetwork.PeerVec[j].NWspeed << "," << PeerNetwork.PeerVec[j].CPU_Usage << "," << MinedInLongestChain << "," << totalMinedBlocks << "," << ratio << "," << PeerNetwork.PeerVec[j].hashing_power << endl;
    }
    NodeDetailsLog.close();

    chdir(currentPath.c_str());
}

void DiscreteEventSimulator ::write_Params_Ratios(Peers &PeerNetwork, string DateTime) // Write node details like z0,z1, Number of nodes mined to a text file
{
    namespace fs = std::experimental::filesystem;
    string currentPath = fs::current_path();
    string directoryName = fs::current_path() / "Logs" / DateTime;
    fs::create_directories(directoryName);
    chdir(directoryName.c_str());

    float MinedInLongestChain, totalMinedBlocks;
    float ratio;

    string arg = "Params_Ratios.txt";
    ofstream NodeDetailsLog(arg);
    // For Finding Ratios of Adversary from the blockchain tree present at honest node
    NodeDetailsLog << "Blocks Mined By Adversary In Blockchain, Total Blocks Mined By Adversary, MPU_Node_Adv" << endl;

    int last = PeerNetwork.PeerVec[1].lastBlockId;
    MinedInLongestChain = 0;
    totalMinedBlocks = 0;
    float Ratio;

    while (PeerNetwork.PeerVec[1].Blockchain[last].minedId != -1)
    {
        if (PeerNetwork.PeerVec[1].Blockchain[last].minedId == 0)
            MinedInLongestChain++;

        last = PeerNetwork.PeerVec[1].Blockchain[last].PrevHash;
    }

    for (auto itr = PeerNetwork.PeerVec[1].Blockchain.begin(); itr != PeerNetwork.PeerVec[1].Blockchain.end(); ++itr)
    {
        if (itr->second.minedId == -1)
            continue;

        if (itr->second.minedId == 0)
            totalMinedBlocks++;
    }
    Ratio = MinedInLongestChain / totalMinedBlocks;
    NodeDetailsLog << MinedInLongestChain << "," << totalMinedBlocks << "," << Ratio << "\n"
                   << endl;

    // For the second Ratio

    float totalBlocks = PeerNetwork.PeerVec[1].Blockchain.size();
    float blocksinmainchain = 0;

    last = PeerNetwork.PeerVec[1].lastBlockId;

    while (PeerNetwork.PeerVec[1].Blockchain[last].minedId != -1)
    {
        blocksinmainchain++;
        last = PeerNetwork.PeerVec[1].Blockchain[last].PrevHash;
    }

    Ratio = blocksinmainchain / totalBlocks;
    NodeDetailsLog
        << "Number of Blocks In Blockchain, Number of Blocks Generated Across All Nodes, MPU_Node_overall" << endl;
    NodeDetailsLog << blocksinmainchain << "," << totalBlocks << "," << Ratio << "\n"
                   << endl;

    NodeDetailsLog << "Simulation Parameters\n--------------------------------------------------------" << endl;
    NodeDetailsLog << "Number Of Peers: " << this->numNodes << endl;
    NodeDetailsLog << "Slow Nodes(z_0): " << this->z_0 << endl;
    NodeDetailsLog << "Low CPU Nodes(z_1): " << this->z_1 << endl;
    NodeDetailsLog << "Adversary Mining Power: " << this->advMinPow << endl;
    NodeDetailsLog << "Adversary Connected to Honest Peers(Tao): " << this->advConPer << endl;
    NodeDetailsLog << "Adversary Type: " << PeerNetwork.PeerVec[0].nodeType << endl;
    chdir(currentPath.c_str());
}
