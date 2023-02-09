#include <iostream>
#include "Event.h"
using namespace std;

Event ::Event(float t, string ty)
{
    eventTime = t;
    type = ty;
}
// class compareTimestamp
// {
// public:
//     bool operator()(const Event &E1, const Event &E2);
// };

Event ::Event(Transaction *T)
{
    this->senderId = T->senderId;
    this->receiverId = T->receiverId;
    this->eventTime = T->txnTime;
    this->type = T->type;
    this->T = T;
    // cout << this->T->senderId << endl;
}