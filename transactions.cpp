
#include <iostream>
#include <sstream>
#include <string.h>

#include "transactions.h"
using namespace std;

Transaction ::Transaction(string Message, float TimeStamp) // Message Format ---- TxnID : A Pays B 10 BTC
{
    vector<string> Splits = Split(Message);

    this->txnId = stoi(Splits[0]);
    this->senderId = stoi(Splits[2]);
    this->type = Splits[3];
    this->receiverId = stoi(Splits[4]);
    this->coins = stof(Splits[5]);
    this->timeStamp = TimeStamp;
};

ostream &operator<<(ostream &out, const Transaction &T)

{
    out << T.txnId << ": " << T.senderId << " " << T.type << " " << T.receiverId << " " << T.coins << " BTC, Timestamp : " << T.timeStamp << endl;
    ;
    return out;
}

vector<string> Transaction ::Split(string Message)
{
    vector<string> splits;
    istringstream iss(Message);
    string word;
    while (iss >> word)
    {
        splits.push_back(word);
    }
    // for (const auto &w : splits)
    // {
    //     cout << w << '\n';
    // }
    return splits;
}
