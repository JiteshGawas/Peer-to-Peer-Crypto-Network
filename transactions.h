#pragma once
#include <iostream>
#include <vector>
#include <string>
// #include "Simulator.h"
using namespace std;

class Transaction
{
public:
    int senderId, receiverId;
    string txnId, type; // for type : Pays , Mines, Init
    float coins, txnTime;

    Transaction(string message, float timeStamp);
    string HashFunction(string Message, float timeStamp);
    vector<string> Split(string Message);
    // void GenerateTransaction(DiscreteEventSimulator &Simulator);
    // void Print();
};
ostream &operator<<(ostream &out, const Transaction &T);
ostream &operator<<(ostream &out, const Transaction *T);
