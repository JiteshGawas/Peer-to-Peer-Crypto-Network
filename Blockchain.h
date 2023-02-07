#pragma once
#include "transactions.h"
#include <vector>

class Block
{
public:
    int blockId, PrevHash, BlockHash;
    vector<Transaction> Transactions;
    vector<float> NodeBalances;
    Block(int blockID, int PrevHash);
};

// class Blockchain
// {
// public:
//     list<Block> Blockchain;
// };
