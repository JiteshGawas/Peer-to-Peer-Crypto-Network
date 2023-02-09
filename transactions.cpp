
#include <iostream>
#include <sstream>
#include <string.h>
#include <functional>

#include "transactions.h"
#include "Simulator.h"
using namespace std;

Transaction ::Transaction(string Message, float TimeStamp)
{
    vector<string> Splits = Split(Message);

    this->txnId = HashFunction(Message, TimeStamp); // function that concatenates Message & TimeStamp information to return a hash of the same
    this->type = Splits[1];                         // Pays, Mines , Init

    if (this->type == "Pays") // Message Format ---- Example : A Pays B 10 BTC
    {
        this->senderId = stoi(Splits[0]);
        this->receiverId = stoi(Splits[2]);
        this->coins = stof(Splits[3]);
        this->txnTime = TimeStamp; //+ rand() % (10);
    }
    else if (this->type == "Mines" || this->type == "Inits") // A Inits 10 BTC or A Mines 10 BTC
    {
        this->senderId = -1; // -1 Represents Coinbase
        this->receiverId = stoi(Splits[0]);
        this->coins = stof(Splits[2]);
        this->txnTime = TimeStamp; //+ rand() % (10);
    }
}

ostream &operator<<(ostream &out, const Transaction &T)

{
    if (T.type == "Pays")
        out << T.txnId << " : " << T.senderId << " " << T.type << " " << T.receiverId << " " << T.coins << " BTC, Timestamp : " << T.txnTime;

    if (T.type == "Mines" || T.type == "Inits")
        out << T.txnId << " : " << T.receiverId << " " << T.type << " " << T.coins << " BTC, Timestamp : " << T.txnTime;

    return out;
}

ostream &operator<<(ostream &out, const Transaction *T)

{
    if (T->type == "Pays")
        out << T->txnId << " : " << T->senderId << " " << T->type << " " << T->receiverId << " " << T->coins << " BTC, Timestamp : " << T->txnTime;

    if (T->type == "Mines" || T->type == "Inits")
        out << T->txnId << " : " << T->receiverId << " " << T->type << " " << T->coins << " BTC, Timestamp : " << T->txnTime;

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

string Transaction ::HashFunction(string Message, float timeStamp)
{

    hash<string> HashProvider;
    string HashMessage = Message + " " + to_string(timeStamp);
    return to_string(HashProvider(HashMessage));
}
