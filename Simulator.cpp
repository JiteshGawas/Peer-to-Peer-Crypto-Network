#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
#include <fstream>
#include <experimental/filesystem>
#include <unistd.h>
using namespace std;

#define MAX_Transactions 5000

DiscreteEventSimulator ::DiscreteEventSimulator(int numPeers, float z0, float z1, float txnMean, float blkMean)
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
    this->terminationTime = 3000; // 60000 ms = 60 seconds 6000 seconds
    // blockInterArrivalMeanTime = 100; // 100*1000 = 1000 seconds
    DateTime = __DATE__ " - " __TIME__;
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
    int eventCounter = 0;

    while (!this->EventQueue.empty()) //
    {
        currEvent = this->EventQueue.top();
        this->EventQueue.pop();
        this->globalTime = currEvent->eventTime;
        eventCounter++;

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
            // cout << "createBlock : Block Created By " << currEvent->senderId << endl;
            PeerNetwork.PeerVec[currEvent->senderId].GenerateBlock(this, &(PeerNetwork.BlockCounter));
            cout << "Wapas" << endl;
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
            PeerNetwork.PeerVec[currEvent->receiverId].ReceiveBlock(this, currEvent, &(PeerNetwork.BlockCounter));
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
            // for (int i = 0; i < itr->second.Transactions.size(); i++)
            //     //     cout << itr->second.Transactions[i].getMessage() << endl;
            //     // cout << "        =======Transactions For The Block==========" << endl;
            //     // for (int i = 0; i < itr->second.Transactions.size(); i++)
            //     // {
            //     //     cout << itr->second.Transactions[i].getMessage() << endl;
            //     // }
            //     /*Printing Node Balances of each Block*/
            //     // cout << "        =======Node Balances For The Block==========" << endl;
            //     // for (int i = 0; i < itr->second.NodeBalances.size(); i++)
            //     // {
            //     //     cout << itr->second.NodeBalances[i] << " ";
            //     // }
            //     // cout << endl;
            //
            // }
            itr++;
        }
        cout << "BlockChain Length of Peer " << i + 1 << " : " << PeerNetwork.PeerVec[i].blockChainLength << endl;
        cout << "All Blocks Including Forks at Peer " << i + 1 << " : " << PeerNetwork.PeerVec[i].Blockchain.size() << endl;
        cout << "Ratio of Blocks in Longest Chain to All Blocks created : " << PeerNetwork.PeerVec[i].blockChainLength / PeerNetwork.PeerVec[i].Blockchain.size() << endl;
    }
    cout << "Event Executed : " << eventCounter << endl;
    cout << "Event Pending in Queue : " << this->EventQueue.size() << endl;
}
// while (!this->EventQueue.empty())
//     {
//         cout << this->EventQueue.top()->type << endl;
//         this->EventQueue.pop();
//     }

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

void DiscreteEventSimulator ::writeBlockChain(Peers &PeerNetwork, string DateTime)
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

void DiscreteEventSimulator ::writeGraphDetails(Peers &PeerNetwork, Graph &adjMatrix, string DateTime)
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

void DiscreteEventSimulator ::writeNodeDetails(Peers &PeerNetwork, string DateTime)
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
    NodeDetailsLog << "Node,Speed(0 - slow | 1 - fast),CPU_Usage(0 - low | 1 - high),#Blocks Mined in Longest Chain, #Total Blocks Mined in Blockchain, Ratio of #BlocksMinedInLongestChain vs #TotalMinedBlocks" << endl;
    for (int j = 0; j < PeerNetwork.numNodes; j++)
    {
        MinedInLongestChain = PeerNetwork.PeerVec[j].getMinedInLongestChain();
        totalMinedBlocks = PeerNetwork.PeerVec[j].getTotalMinedBlocks();
        ratio = MinedInLongestChain / totalMinedBlocks;
        NodeDetailsLog << j + 1 << "," << PeerNetwork.PeerVec[j].NWspeed << "," << PeerNetwork.PeerVec[j].CPU_Usage << "," << MinedInLongestChain << "," << totalMinedBlocks << "," << ratio << endl;
    }
    NodeDetailsLog.close();

    chdir(currentPath.c_str());
}
