#pragma once
#include "transactions.h"

class Event
{
public:
    float eventTime;
    string type;
    int senderId, receiverId;
    Transaction *T;
    // Block *B;

    Event(float t, string ty);
    Event(Transaction *T); //&T Previously
};
