#pragma once
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Transaction
{
public:
    int txnId, senderId, receiverId;
    string type;
    float coins, timeStamp;

    Transaction(string message, float timeStamp);
    vector<string> Split(string Message);
    // void Print();
};
ostream &operator<<(ostream &out, const Transaction &T);
