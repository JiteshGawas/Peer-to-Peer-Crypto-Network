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

#define MessageSize 1000

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

    priority_queue<Event *, vector<Event *>, compareTimestamp> EventQueue;
    Event *currEvent;

    DiscreteEventSimulator(int a, float b, float c);
    void PrintParameters();
    void startSimulation(Graph &adjMatrix, Peers &PeerNetwork);
    friend class Node;
};

//-----------------------------------------For Blocks---------------------------------------------------------------

class Block
{
public:
    int blockId, PrevHash, BlockHash;
    vector<Transaction> Transactions;
    vector<float> NodeBalances;
    Block(int blockID, int PrevHash);
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
    float eventTime;
    string type;
    int senderId, receiverId;
    Transaction T;
    // Block *B;

    Event(Node *, float t, string ty);
    Event(Transaction *T, string EventType, Node *sender, Node *receiver, float prop_delay); //&T Previously
    Event(Transaction T, string EventType, Node *sender, Node *receiver, float prop_delay);  //&T Previously
    float calculate_Latency(int, int);
};

//------------------------------------------For Graph---------------------------------------------------------------

class Graph
{
public:
    vector<vector<int>> adjMatrix;
    vector<int> visited;
    int numNodes, minDegree, maxDegree;

    Graph(int numNodes, int minDegree, int maxDegree);
    void createGraph();
    bool isConnected();
    void dfs(int v);
    bool does_exist(const vector<vector<int>> &adjList, int row, int item);
    friend class Peer;
};

//-----------------------------------------For Peers---------------------------------------------------------------

class Node
{
public:
    int NodeId, NWspeed, CPU_Usage;
    float balance;
    float hashing_power;
    vector<Node *> connectedPeers;

    // map<string, Transaction *> AllTransactions;
    map<string, Transaction> AllTransactions;
    map<int, Block *> Blockchain;
    // NWspeed : 0 (Slow) 1(Fast) | CPU_Usage = 0 (low_cpu) 1(high_cpu)
    bool isConnected(const Graph &adjMatrix, int peerId)
    {
        if (adjMatrix.adjMatrix[this->NodeId][peerId])
            return true;
        else
            return false;
    }
    void GenerateTransaction(DiscreteEventSimulator *Simulator, string TxnType);
    void ReceiveTransaction(DiscreteEventSimulator *Simulator, Event *currEvent);
    float RandomInterArrivalTxnTime();
};

class Peers
{
public:
    vector<Node> PeerVec; // Vector of PeerVec
    int numNodes;
    set<int> z0_Set, z1_Set;
    float slow_HashPower;

    Peers(int numNodes, DiscreteEventSimulator &Simulator);
    void PeerInfo();
    void setConnectedPeers(Graph &adjMatrix);
};
