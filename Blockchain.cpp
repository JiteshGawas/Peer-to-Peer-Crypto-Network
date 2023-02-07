#include "Blockchain.h"
#include <iostream>

Block::Block(int blockID, int PrevHash)
{
    this->blockId = blockID;
    this->BlockHash = blockID;
    this->PrevHash = PrevHash;
}