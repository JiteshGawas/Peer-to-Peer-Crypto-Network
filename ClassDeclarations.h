#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <queue>
#include <string>
#include <sstream>
#include <string.h>
#include <functional>
using namespace std;

//-----------------------------------------Forward Declarations---------------------------------------------------------------
class DiscreteEventSimulator;
class Block;
class Transaction;
class Event;
class compareTimestamp;
class Graph;
class Node;
class Peers;

//-----------------------------------------For Simulator---------------------------------------------------------------
class compareTimestamp
{
public:
    bool operator()(const Event *E1, const Event *E2);
};

class DiscreteEventSimulator
{
public:
    int numNodes;
    float z_0, z_1, prop_delay;
    float globalTime = 0;
    int terminationTime;
    int transaction_Counter = 0;
    float txnInterArrivalMeanTime;
    float blockInterArrivalMeanTime;
    float advMinPow;
    float advConPer;
    string DateTime;

    priority_queue<Event *, vector<Event *>, compareTimestamp> EventQueue;
    Event *currEvent;

    DiscreteEventSimulator(int numPeers, float z0, float z1, float advMinPow, float advConPer, float txnMean, float blkMean, string advType);
    void PrintParameters();
    void startSimulation(Graph &adjMatrix, Peers &PeerNetwork);
    void writeBlockArrivalTimes(Peers &PeerNetwork, string DateTime);
    void writeBlockChain(Peers &PeerNetwork, string DateTime);
    void writeGraphDetails(Peers &PeerNetwork, Graph &adjMatrix, string DateTime);
    void writeNodeDetails(Peers &PeerNetwork, string DateTime);
    void write_Params_Ratios(Peers &PeerNetwork, string DateTime);
    friend class Node;
};

//-----------------------------------------For Blocks---------------------------------------------------------------

class Block
{
public:
    int blockId, PrevHash, BlockHash;
    int minedId, blockLevel = 0;
    float minedTime = 1.0;
    vector<Transaction> Transactions;
    vector<float> NodeBalances;

    Block();
    Block(int blockID, int PrevHash, int blockLevel);

    void operator=(const Block *rhs);
    void operator=(const Block &rhs);
};

//-----------------------------------------For transactions---------------------------------------------------------------
class Transaction
{
public:
    int senderId, receiverId;
    string txnId, type; // for type : Pays , Mines, Init
    float coins, txnTime;

    Transaction();
    Transaction(string message, float timeStamp);
    string HashFunction(string Message, float timeStamp);
    string getMessage();
    vector<string> Split(string Message);
    void operator=(const Transaction *rhs);
    void operator=(const Transaction &rhs);

    // void GenerateTransaction(DiscreteEventSimulator &Simulator);
    // void Print();
};

ostream &operator<<(ostream &out, const Transaction &T);
ostream &operator<<(ostream &out, const Transaction *T);

//-----------------------------------------For Event---------------------------------------------------------------

class Event
{
public:
    int senderId, receiverId;
    float eventTime;
    string type;
    Transaction T;
    Block B;
    Event(int NodeId, float eventTime, string type);
    Event(Transaction *T, string EventType, Node *sender, Node *receiver, float prop_delay); //&T Previously
    Event(Transaction T, string EventType, Node *sender, Node *receiver, float prop_delay);  //&T Previously
    Event(Block B, float eventTime, string EventType, Node *sender, Node *receiver);
    Event(Block *B, float eventTime, string EventType, Node *sender, Node *receiver);
    float calculate_Latency(int senderNWspeed, int receiverNWspeed, int numTransaction = 1);
};

//------------------------------------------For Graph---------------------------------------------------------------

class Graph
{
public:
    vector<vector<int>> adjMatrix;
    vector<int> visited;
    int numNodes, minDegree, maxDegree;

    Graph(int numNodes, int minDegree, int maxDegree);
    void createGraph(float advMinPow, int numHonest);
    bool isConnected();
    void dfs(int v);
    bool does_exist(const vector<vector<int>> &adjList, int row, int item);
    friend class Peer;
};

//-----------------------------------------For Peers---------------------------------------------------------------

class Node
{
public:
    int NodeId, NWspeed, CPU_Usage, lastBlockId; // NWspeed : 0 (Slow) 1(Fast) | CPU_Usage = 0 (low_cpu) 1(high_cpu)
    float balance;
    float hashing_power;
    int blockChainLength = 0;
    int privateChainLength = 0;
    int Selfishflag = 0;
    string nodeType; // hon|adv
    vector<Node *> connectedPeers;

    queue<Block> privateBlocks;
    int lastBlockIdPrivate = 0;
    // map<string, Transaction *> AllTransactions;
    map<string, Transaction> AllTransactions;
    map<string, Transaction> PendingTransaction;
    map<int, Block> Blockchain;
    map<int, Block> PendingBlocks;
    map<int, bool> ReceivedBlocks;
    map<int, float> BlockArrivalTimes;

    bool isConnected(const Graph &adjMatrix, int peerId)
    {
        if (adjMatrix.adjMatrix[this->NodeId][peerId])
            return true;
        else
            return false;
    }
    void GenerateTransaction(DiscreteEventSimulator *Simulator, string TxnType);
    void ReceiveTransaction(DiscreteEventSimulator *Simulator, Event *currEvent);
    float RandomInterArrivalTxnTime(float InterArrivalMean);
    float RandomInterArrivalBlockTime(float InterArrivalMean);
    void GenerateBlock(DiscreteEventSimulator *Simulator, int *BlockCounter);
    void MineBlock(DiscreteEventSimulator *Simulator, Event *E, int *BlockCounter);
    void BroadcastBlock(DiscreteEventSimulator *Simulator, Event *E);
    void ReceiveBlock(DiscreteEventSimulator *Simulator, Event *E, int *BlockCounter);
    bool VerifyAddBlock(Block B); // true if block verified and successfullly, false otherwise
    int getMinedInLongestChain();
    int getTotalMinedBlocks();

    //--------For Attacker----------
    // void MinePrivate(DiscreteEventSimulator *Simulator, Event *E, int *BlockCounter);
    void BroadcastPrivateBlock(DiscreteEventSimulator *Simulator);
    // void ReceiveSelfish(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter); // mayBeRedundant
    void SelfishAttack(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter);
    void StubbornAttack(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter);
    // void addPrivateBlock(DiscreteEventSimulator *Simulator);
};

class Peers
{
public:
    vector<Node> PeerVec; // Vector of PeerVec
    int numNodes, BlockCounter, numHonest;
    set<int> z0_Set, z1_Set;
    float slow_HashPower;

    Peers(int numNodes, DiscreteEventSimulator &Simulator, string attackerType);
    void PeerInfo();
    void setConnectedPeers(Graph &adjMatrix);
};
