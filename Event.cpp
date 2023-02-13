#include "ClassDeclarations.h"
#include <iostream>
#include <chrono>
using namespace std;

// Event ::Event(Node *Generator, float eventTime, string type)
// {
//     this->eventTime = eventTime;
//     this->type = type;
//     senderId = Generator->NodeId;
//     receiverId = -1;
//     // T = NULL;
// }

Event ::Event(int NodeId, float eventTime, string type)
{
    this->senderId = NodeId;
    this->receiverId = -1;
    this->type = type;
    this->eventTime = eventTime;
}

// class compareTimestamp
// {
// public:
//     bool operator()(const Event &E1, const Event &E2);
// };

Event ::Event(Transaction *T, string EventType, Node *sender, Node *receiver, float prop_delay)
{
    this->senderId = sender->NodeId;
    this->receiverId = receiver->NodeId;
    this->eventTime = T->txnTime + prop_delay + calculate_Latency(sender->NWspeed, receiver->NWspeed);
    this->type = EventType;
    this->T = T;
    // cout << this->T->senderId << endl;
}

Event ::Event(Transaction T, string EventType, Node *sender, Node *receiver, float prop_delay)
{
    this->senderId = sender->NodeId;
    this->receiverId = receiver->NodeId;
    this->eventTime = T.txnTime + prop_delay + calculate_Latency(sender->NWspeed, receiver->NWspeed);
    this->type = EventType;
    this->T = T;
    // cout << this->T->senderId << endl;
}

Event ::Event(Block B, float eventTime, string EventType, Node *sender, Node *receiver)
{

    this->senderId = sender->NodeId;
    if (receiver == NULL)
        this->receiverId = -1;
    else
        this->receiverId = receiver->NodeId;

    this->eventTime = eventTime + calculate_Latency(sender->NWspeed, receiver->NWspeed, B.Transactions.size()); // RandomInterArrivalTime not latency
    this->type = EventType;
    this->B = B;
    // cout << this->T->senderId << endl;
}

Event ::Event(Block *B, float eventTime, string EventType, Node *sender, Node *receiver)
{

    this->senderId = sender->NodeId;
    this->B = B;
    this->type = EventType;
    if (receiver == NULL)
    {
        this->receiverId = -1;
        this->eventTime = eventTime;
    }
    else
    {
        this->receiverId = receiver->NodeId;
        this->eventTime = eventTime + calculate_Latency(sender->NWspeed, receiver->NWspeed, this->B.Transactions.size());
    }

    // RandomInterArrivalTime not latency if receiver is not there - receiver.nwspeed giving segmentation fault

    // cout << this->T->senderId << endl;
}

float Event ::calculate_Latency(int senderNWspeed, int receiverNWspeed, int numTransaction)
{
    float c, d;
    float latency;
    int MessageSize = 8192; // 1024 B in Bits - one Transaction assumed to be of 1024 B
    if (senderNWspeed && receiverNWspeed)
    {
        c = 100e6;
    }
    else
    {
        c = 5e6;
    }

    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    exponential_distribution<float> dij(0.1);

    d = dij(generator);

    latency = ((MessageSize * 1) / c) + d; // multiply by numTransaction

    return latency;
}