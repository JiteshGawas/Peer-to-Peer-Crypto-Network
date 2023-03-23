#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
#include <set>
#include <map>
#include <sstream>
#include <string>

Peers::Peers(int numNodes, DiscreteEventSimulator &Simulator, string attackerType)
{
    this->numNodes = numNodes;
    this->numHonest = this->numNodes - 1;
    srand(time(0));

    PeerVec.resize(Simulator.numNodes);
    for (int i = 0; i < Simulator.numNodes; i++)
    {
        PeerVec[i].NodeId = i;
        PeerVec[i].balance = 30 + rand() % (71); // add some random numbers of BTC between 30-50 for each peer
        PeerVec[i].NWspeed = 1;
        PeerVec[i].CPU_Usage = 1;
        Block B(1, 0, 1);
        PeerVec[i].Blockchain.insert({1, B}); // Genesis Block
        PeerVec[i].Blockchain[1].blockLevel = 1;
        PeerVec[i].Blockchain[1].minedId = -1;
        PeerVec[i].blockChainLength = 1;
        PeerVec[i].lastBlockId = 1; // Which Is Genesis Block

        if (i == 0)
        {
            PeerVec[i].nodeType = attackerType;
            PeerVec[i].hashing_power = Simulator.advMinPow;
            PeerVec[i].privateChainLength = 1;
            PeerVec[i].lastBlockIdPrivate = 1;
            PeerVec[i].Selfishflag = 1;
        }
        else
        {
            PeerVec[i].nodeType = "hon";
        }
    }
    this->BlockCounter = 1;
    int temp;
    // Randomly Setting z0 % Nodes to have slowNWSpeed
    while (z0_Set.size() < (Simulator.z_0 * this->numHonest))
    {
        temp = rand() % Simulator.numNodes;
        if (temp == 0)
            continue;
        z0_Set.insert(temp); // cout<<"Z0 peers are"<<endl;
    }
    for (auto it = z0_Set.begin(); it != z0_Set.end(); ++it)
    {
        PeerVec[*it].NWspeed = 0;
    }

    // Randomly Setting z1 % Nodes to be of low_cpu
    while (z1_Set.size() < (Simulator.z_1 * this->numHonest))
    {
        temp = rand() % Simulator.numNodes;
        if (temp == 0)
            continue;
        z1_Set.insert(temp);
    }
    for (auto it = z1_Set.begin(); it != z1_Set.end(); ++it)
        PeerVec[*it].CPU_Usage = 0;

    // Distributing Hashing Powes Between Peers Such That "High_Cpu" Nodes Have 10 times more hashing power than "Low_Cpu" Nodes
    this->slow_HashPower = (1 - Simulator.advMinPow) / ((this->numHonest * Simulator.z_1) + (this->numHonest - (this->numHonest * Simulator.z_1)) * 10);
    // cout << "SlowHashPower - " << this->slow_HashPower << " | AdVMinPower - " << Simulator.advMinPow << endl;
    for (int i = 1; i < Simulator.numNodes; i++) // starting from 1, as 0-node is adversary, mining power assigned already
    {
        if (this->PeerVec[i].CPU_Usage == 0) // i.e It is a low_CPU Node
            this->PeerVec[i].hashing_power = this->slow_HashPower;

        else
            this->PeerVec[i].hashing_power = this->slow_HashPower * 10;
    }

    for (int i = 0; i < this->numNodes; i++)
    {

        for (int j = 0; j < this->numNodes; j++)
        {
            PeerVec[i].Blockchain[1].NodeBalances.push_back(PeerVec[j].balance);
        }
    }
}

void Peers::PeerInfo()
{
    cout << "PeerId  Balance  NWSpeed  CPU_Usage Node Type Hashing Power" << endl;
    for (int i = 0; i < numNodes; i++)
    {
        cout << PeerVec[i].NodeId << "\t " << PeerVec[i].balance << "\t  " << PeerVec[i].NWspeed << "\t  " << PeerVec[i].CPU_Usage << "\t " << PeerVec[i].nodeType << "\t" << PeerVec[i].hashing_power << endl;

        cout << endl;
    }

    // To Check Sum Of Hashing Powers Of All The Nodes = 1
    float sum = 0;
    for (int i = 0; i < numNodes; i++)
    {
        sum += this->PeerVec[i].hashing_power;
    }
    if (sum)
        cout << "Hashing Power Distributed Correctly" << endl;
    else
        cout << "Sum Is : " << sum << endl;
}

void Peers ::setConnectedPeers(Graph &adjMatrix)
{
    for (int i = 0; i < this->numNodes; i++)
    {
        for (int j = 0; j < this->numNodes; j++)
        {
            if (adjMatrix.adjMatrix[i][j] == 1 && i != j)
            {
                this->PeerVec[i].connectedPeers.push_back(&PeerVec[j]);
            }
        }
    }
}

float Node ::RandomInterArrivalTxnTime(float InterArrivalTxnMean)
{
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);

    exponential_distribution<float> InterArrivalMeanTime(1 / InterArrivalTxnMean); // 1/20 i.e exp dist with mean as 20 seconds

    return InterArrivalMeanTime(generator);
}

float Node ::RandomInterArrivalBlockTime(float InterArrivalBlockMean)
{
    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    exponential_distribution<float> InterArrivalMeanTime(1 / (InterArrivalBlockMean / this->hashing_power)); // 1/(I/Hk) i.e exp dist with mean as I/Hk seconds

    return InterArrivalMeanTime(generator);
}

void Node ::GenerateTransaction(DiscreteEventSimulator *Simulator, string TxnType)
{
    // srand(time(0));
    int coins = 5 + rand() % 30;
    int receiverID = rand() % Simulator->numNodes;

    while (this->NodeId == receiverID)
        receiverID = rand() % Simulator->numNodes;

    stringstream ss;

    // A Pays B 60 BTC
    if (TxnType == "Pays")
    {
        ss << this->NodeId << " " << TxnType << " " << receiverID << " " << coins << " BTC";
    }

    // A Mines 60 BTC or A Inits 60 BTC
    if (TxnType == "Mines" || TxnType == "Inits")
    {
        coins = 50;
        receiverID = this->NodeId;
        ss << this->NodeId << " " << TxnType << " "
           << " " << coins << " BTC"; // A Inits
    }

    string Message = ss.str();

    Transaction *T;
    if (TxnType == "Pays")
    {
        if (Simulator->transaction_Counter == 0)
        {
            T = new Transaction(Message, 0);
        }
        else
        {
            T = new Transaction(Message, Simulator->globalTime + RandomInterArrivalTxnTime(Simulator->txnInterArrivalMeanTime));
        }

        this->AllTransactions[T->txnId] = T;
        this->PendingTransaction[T->txnId] = T;

        for (int i = 0; i < this->connectedPeers.size(); i++)
        {
            Simulator->EventQueue.push(new Event(T, "txn_Receive", this, this->connectedPeers[i], Simulator->prop_delay));
        }

        delete T;

        if (Simulator->transaction_Counter != 0)
            Simulator->transaction_Counter++;

        Simulator->EventQueue.push(new Event(this->NodeId, Simulator->globalTime + RandomInterArrivalTxnTime(Simulator->txnInterArrivalMeanTime), "txn_Generate"));
    }
}

void Node ::ReceiveTransaction(DiscreteEventSimulator *Simulator, Event *currEvent)
{
    if (this->AllTransactions.find(currEvent->T.txnId) == this->AllTransactions.end()) // transactions not in the allTransaction pool
    {
        this->AllTransactions[currEvent->T.txnId] = currEvent->T;
        this->PendingTransaction[currEvent->T.txnId] = currEvent->T;

        for (int i = 0; i < this->connectedPeers.size(); i++) // send to connected peers except the receiver
        {
            if (this->connectedPeers[i]->NodeId == currEvent->senderId) // For ensuring loop-less forwarding
                continue;

            Simulator->EventQueue.push(new Event(currEvent->T, "txn_Receive", this, this->connectedPeers[i], Simulator->prop_delay));
        }
    }
}

void Node ::GenerateBlock(DiscreteEventSimulator *Simulator, int *BlockCounter)
{

    int PrevBlockId, BlockLevel;
    PrevBlockId = this->lastBlockId;
    BlockLevel = this->Blockchain[this->lastBlockId].blockLevel + 1;

    Block *B = new Block(++(*BlockCounter), PrevBlockId, BlockLevel);
    int count = 500;
    B->minedId = this->NodeId;
    // cout << "Block is Created by the Node " << B->minedId << endl;
    B->NodeBalances = this->Blockchain[lastBlockId].NodeBalances; // Taking Balances from prev blocks

    for (auto itr = this->PendingTransaction.begin(); itr != this->PendingTransaction.end() && this->PendingTransaction.size() != 1; ++itr)
    {

        if (B->NodeBalances[itr->second.senderId] >= itr->second.coins) // Balance of sender > Txn sender coins
        {
            B->Transactions.push_back(itr->second);

            count--;
            B->NodeBalances[itr->second.senderId] -= itr->second.coins;
            B->NodeBalances[itr->second.receiverId] += itr->second.coins;
            this->PendingTransaction.erase(itr++);
        }

        if (count == 0)
            break;
    }

    Simulator->EventQueue.push(new Event(B, Simulator->globalTime + Simulator->prop_delay + this->RandomInterArrivalBlockTime(Simulator->blockInterArrivalMeanTime), "MineBlock", this, NULL));
    delete B;
}

void Node::MineBlock(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter)
{

    if (this->blockChainLength + 1 != currEvent->B.blockLevel) // because the block was supposed to mine at the nextLevel
        return;
    string coinbaseMessage = to_string(this->NodeId) + " Mines 50 BTC";
    Transaction T(coinbaseMessage, currEvent->eventTime);
    currEvent->B.Transactions.push_back(T);
    currEvent->B.NodeBalances[this->NodeId] += 50;

    this->Blockchain.insert({currEvent->B.blockId, currEvent->B});
    // this->Blockchain[currEvent->B.blockId].minedTime = currEvent->eventTime;
    if (currEvent->B.blockLevel == this->blockChainLength + 1)
    {
        this->blockChainLength++;
        this->lastBlockId = currEvent->B.blockId;
    }
    // after block is mined add an coinbase txn + 50 coins to the miner

    if (this->nodeType == "hon")
    {
        this->BroadcastBlock(Simulator, currEvent);
    }
    else
    {
        if (Selfishflag == 1)
        {
            this->BroadcastBlock(Simulator, currEvent);
            Selfishflag = 0;
        }
        else
            this->privateBlocks.push(currEvent->B);
    }

    this->GenerateBlock(Simulator, BlockCounter);
}

void Node ::BroadcastBlock(DiscreteEventSimulator *Simulator, Event *currEvent)
{
    for (int i = 0; i < this->connectedPeers.size(); i++)
    {
        if (this->connectedPeers[i]->NodeId != currEvent->senderId) // LoopLess Forwarding
            Simulator->EventQueue.push(new Event(currEvent->B, Simulator->globalTime + Simulator->prop_delay, "ReceiveBlock", this, this->connectedPeers[i]));
    }
}

void Node ::ReceiveBlock(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter)
{
    bool flag;

    if (this->ReceivedBlocks.find(currEvent->B.blockId) != this->ReceivedBlocks.end()) // i.e Block already Received
    {
        this->GenerateBlock(Simulator, BlockCounter);
        return;
    }
    this->BlockArrivalTimes[currEvent->B.blockId] = currEvent->eventTime;

    this->ReceivedBlocks[currEvent->B.blockId] = true;

    int parentHash = currEvent->B.PrevHash;

    if (this->Blockchain.find(parentHash) != this->Blockchain.end()) // Parent is found in blockchain
    {
        flag = this->VerifyAddBlock(currEvent->B); // Parent Found so verify & add Block
        if (flag)
        {
            if (this->nodeType == "hon")
            {
                this->BroadcastBlock(Simulator, currEvent);
            }
            else if (this->nodeType == "self")
            {
                this->SelfishAttack(Simulator, currEvent, BlockCounter);
            }
            else if (this->nodeType == "stub")
            {
                this->StubbornAttack(Simulator, currEvent, BlockCounter);
            }
        }

        // The Block was verified and added successfully
        Block TempBlock = currEvent->B;
        while (flag && (this->PendingBlocks.find(TempBlock.blockId) != this->PendingBlocks.end()))
        {
            flag = this->VerifyAddBlock(this->PendingBlocks[TempBlock.blockId]);
            if (flag)
            {
                if (this->nodeType == "hon")
                {
                    this->BroadcastBlock(Simulator, currEvent);
                }
                else if (this->nodeType == "self")
                {
                    this->SelfishAttack(Simulator, currEvent, BlockCounter);
                }
            }
            TempBlock = this->PendingBlocks[TempBlock.blockId];
            // verify and add this block to blockchain
        }

        // search if this newly added block is a parent of some block which are left in nonverifiedblocks
    }

    else // Parent not found in blockchain, therefore including it in pendingBlocks
    {
        this->PendingBlocks.insert({currEvent->B.PrevHash, currEvent->B});
    }
    this->GenerateBlock(Simulator, BlockCounter);
}

bool Node ::VerifyAddBlock(Block B) // true if block verified and successfullly, false otherwise
{
    vector<float> tempNodeBalance = this->Blockchain[B.PrevHash].NodeBalances;
    int flag = 0;
    for (int i = 0; i < B.Transactions.size(); ++i)
    {

        if (B.Transactions[i].type == "Mines") // when its a coinbase transaction just add 50 BTC to the miner
        {
            tempNodeBalance[B.minedId] += 50;
            continue;
        }
        tempNodeBalance[B.Transactions[i].senderId] -= B.Transactions[i].coins;
        tempNodeBalance[B.Transactions[i].receiverId] += B.Transactions[i].coins;
    }

    for (int i = 0; i < tempNodeBalance.size(); i++)
    {
        if (tempNodeBalance[i] == B.NodeBalances[i])
        {
            continue;
        }
        else
        {
            flag = 1;
            break;
        }
    }
    if (flag == 0) // Block is valid
    {
        for (int i = 0; i < B.Transactions.size(); ++i)
        {
            // Balance of sender > Txn sender coins
            auto itr = this->PendingTransaction.find(B.Transactions[i].txnId);
            if (itr != this->PendingTransaction.end()) // TXN Is in pending transactions, therfore delete it from there
            {
                this->PendingTransaction.erase(B.Transactions[i].txnId);
            }
        }
        this->Blockchain.insert({B.blockId, B}); // Added to Blockchain
        this->PendingBlocks.erase(B.PrevHash);
        if (this->blockChainLength + 1 == B.blockLevel) // to keep the blockchain length consistent in case forks happens
        {
            this->blockChainLength++;
            this->lastBlockId = B.blockId;
        }
        return true;
    }
    this->PendingBlocks.erase(B.PrevHash);
    return false;
}

int Node::getMinedInLongestChain() // Number of blocks mined in the longest chain
{
    int last = this->lastBlockId;
    int MinedInLongest = 0;

    while (this->Blockchain[last].minedId != -1)
    {
        if (this->Blockchain[last].minedId == this->NodeId)
            MinedInLongest++;

        last = this->Blockchain[last].PrevHash;
    }

    return MinedInLongest;
}

int Node ::getTotalMinedBlocks() // Number of blocks mined in the total blockchain
{
    int totalMinedBlocks = 0;
    for (auto itr = this->Blockchain.begin(); itr != this->Blockchain.end(); ++itr)
    {
        if (itr->second.minedId == -1)
            continue;

        if (itr->second.minedId == this->NodeId)
            totalMinedBlocks++;
    }

    return totalMinedBlocks;
}

//---------------------------------------For Adversary-----------------------------------

void Node ::BroadcastPrivateBlock(DiscreteEventSimulator *Simulator)
{
    for (int i = 0; i < this->connectedPeers.size(); i++)
    {

        Simulator->EventQueue.push(new Event(this->privateBlocks.front(), Simulator->globalTime + Simulator->prop_delay, "ReceiveBlock", this, this->connectedPeers[i]));
    }
}

void Node ::SelfishAttack(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter)
{
    if (this->privateBlocks.size() == 0)
    {
        // this->lastBlockIdPrivate = this->lastBlockId;
        Selfishflag = 0;
        this->GenerateBlock(Simulator, BlockCounter);
    }
    else if (this->privateBlocks.size() == 1) // when lead becomes 0 : broadcast the mined block, lead = 1: broadcast all the blocks
    {

        this->BroadcastPrivateBlock(Simulator); // add to self blockchain also : Look at verifyAdd function
        Selfishflag = 1;
        this->GenerateBlock(Simulator, BlockCounter);
        this->privateBlocks.pop();
    }

    else if (this->privateBlocks.size() == 2)
    {
        while (!privateBlocks.empty())
        {
            this->BroadcastPrivateBlock(Simulator);
            // add to self blockchain also : Look at verifyAdd function

            this->privateBlocks.pop();
        }
    }
    else if (this->privateBlocks.size() > 2)
    {
        this->BroadcastPrivateBlock(Simulator); // add to self blockchain also : Look at verifyAdd function

        this->GenerateBlock(Simulator, BlockCounter);
        this->privateBlocks.pop();
    }

    // if (this->privateBlocks.size() > 2)
    // {
    //     this->BroadcastPrivateBlock(Simulator);
    //     this->privateBlocks.pop();
    // }
}

void Node ::StubbornAttack(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter)
{

    if (this->privateBlocks.size() >= 1)
    {
        this->BroadcastPrivateBlock(Simulator); // add to self blockchain also : Look at verifyAdd function
        this->GenerateBlock(Simulator, BlockCounter);
        this->privateBlocks.pop();
    }
    else
    {
        this->GenerateBlock(Simulator, BlockCounter);
    }
}