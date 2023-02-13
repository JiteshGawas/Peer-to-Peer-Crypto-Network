#include "ClassDeclarations.h"
#include <iostream>

Block::Block(int blockID, int PrevHash)
{
    this->blockId = blockID;
    this->BlockHash = blockID;
    this->PrevHash = PrevHash;
    
}

Block ::Block()
{
    this->BlockHash = -1;
    this->blockId = -1;
    this->minedId = -1;
    this->PrevHash = -1;
}


void Block ::operator=(const Block *rhs)
{

    this->blockId = rhs->blockId;
    this->minedId = rhs->minedId;
    this->BlockHash = rhs->BlockHash;
    this->NodeBalances = rhs->NodeBalances;
    this->PrevHash = rhs->PrevHash;
    this->Transactions = rhs->Transactions;
}

void Block ::operator=(const Block &rhs)
{
    this->blockId = rhs.blockId;
    this->minedId = rhs.minedId;
    this->BlockHash = rhs.BlockHash;
    this->NodeBalances = rhs.NodeBalances;
    this->PrevHash = rhs.PrevHash;
    this->Transactions = rhs.Transactions;
}