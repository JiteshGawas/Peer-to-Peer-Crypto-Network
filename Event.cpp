#include "ClassDeclarations.h"
#include <iostream>
#include <chrono>
using namespace std;

Event ::Event(Node *Generator, float t, string ty)
{
    eventTime = t;
    type = ty;
    senderId = Generator->NodeId;
    receiverId = -1;
    // T = NULL;
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

float Event ::calculate_Latency(int senderNWspeed, int receiverNWspeed)
{
    float c, d;
    float latency;
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

    latency = (MessageSize / c) + d;

    return latency;
}