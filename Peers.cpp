#include "ClassDeclarations.h"
#include <iostream>
#include <random>
#include <chrono>
#include <set>
#include <map>
#include <sstream>
#include <string>

Peers::Peers(int numNodes, DiscreteEventSimulator &Simulator)
{
    this->numNodes = numNodes;
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
    }
    this->BlockCounter = 1;
    // Randomly Setting z0 % Nodes to have slowNWSpeed
    while (z0_Set.size() < (Simulator.z_0 * Simulator.numNodes))
    {
        z0_Set.insert(rand() % Simulator.numNodes); // cout<<"Z0 peers are"<<endl;
    }
    for (auto it = z0_Set.begin(); it != z0_Set.end(); ++it)
    {
        PeerVec[*it].NWspeed = 0;
    }

    // Randomly Setting z1 % Nodes to be of low_cpu
    while (z1_Set.size() < (Simulator.z_1 * Simulator.numNodes))
        z1_Set.insert(rand() % Simulator.numNodes);
    for (auto it = z1_Set.begin(); it != z1_Set.end(); ++it)
        PeerVec[*it].CPU_Usage = 0;

    // Distributing Hashing Powes Between Peers Such That "High_Cpu" Nodes Have 10 times more hashing power than "Low_Cpu" Nodes
    this->slow_HashPower = 1 / ((this->numNodes * Simulator.z_1) + (this->numNodes - (this->numNodes * Simulator.z_1)) * 10);

    for (int i = 0; i < Simulator.numNodes; i++)
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
    cout << "PeerId  Balance  NWSpeed  CPU_Usage Hashing Power" << endl;
    // cout << "BlockID, PrevHash, Blockhash" << endl;
    for (int i = 0; i < numNodes; i++)
    {
        // map<int, Block *>::iterator it = PeerVec[i].Blockchain.begin();

        // Iterate through the map and print the elements
        // while (it != PeerVec[i].Blockchain.end())
        // {
        //     std::cout << "BlockId: " << it->second->blockId << ", PrevHash: " << it->second->PrevHash << ", BlockHash: " << it->second->BlockHash << std::endl;
        //     ++it;
        // }
        cout << PeerVec[i].NodeId << "\t " << PeerVec[i].balance << "\t  " << PeerVec[i].NWspeed << "\t  " << PeerVec[i].CPU_Usage << "\t " << PeerVec[i].hashing_power << endl;
        cout << "Genesis Block Balances For Node " << i << endl;
        for (int j = 0; j < this->numNodes; j++)
        {
            cout << PeerVec[i].Blockchain[1].NodeBalances[j] << " ";
        }
        cout << endl;
    }

    // To Check Sum Of Hashing Powers Of All The Nodes = 1
    float sum = 0;
    for (int i = 0; i < numNodes; i++)
    {
        sum += this->PeerVec[i].hashing_power;
    }
    if (sum == 1)
        cout << "Hashing Power Distributed Correctly" << endl;
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

    // for (int i = 0; i < this->numNodes; i++)
    // {
    //     cout << i << " -> ";
    //     for (int j = 0; j < this->PeerVec[i].connectedPeers.size(); j++)
    //     {

    //         cout << " " << this->PeerVec[i].connectedPeers[j]->NodeId;
    //     }
    //     cout << endl;
    // }
    // cout << endl;
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
            // cout << "Initial Transaction" << endl;
            T = new Transaction(Message, 0);
        }
        else
        {
            // cout << "Transaction From Inside While" << endl;
            T = new Transaction(Message, Simulator->globalTime + RandomInterArrivalTxnTime(Simulator->txnInterArrivalMeanTime));
        }

        this->AllTransactions[T->txnId] = T;
        this->PendingTransaction[T->txnId] = T;

        for (int i = 0; i < this->connectedPeers.size(); i++)
        {
            Simulator->EventQueue.push(new Event(T, "txn_Receive", this, this->connectedPeers[i], Simulator->prop_delay));
        }

        delete T;

        // std::map<std::string, Transaction>::iterator it = this->AllTransactions.begin();

        // while (it != this->AllTransactions.end())
        // {
        //     std::cout << "txnID: " << it->first << ", TxnObject: " << it->second << std::endl;
        //     ++it;
        // }
        if (Simulator->transaction_Counter != 0)
            Simulator->transaction_Counter++;
        // cout << Simulator->transaction_Counter << endl;

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
    Block *B = new Block(++(*BlockCounter), this->lastBlockId, this->blockChainLength + 1);
    int count = 500;
    B->minedId = this->NodeId;
    // cout << "Block is Created by the Node " << B->minedId << endl;
    B->NodeBalances = this->Blockchain[lastBlockId].NodeBalances; // Taking Balances from prev blocks

    for (auto itr = this->PendingTransaction.begin(); itr != this->PendingTransaction.end() && this->PendingTransaction.size() != 1; ++itr)
    {
        // cout << "Yaha Hoon Atka : TransSIze = " << this->PendingTransaction.size() << endl;
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
        // cout << "Uske Baad Yaha" << endl;
    }

    // cout << "\nBalance Of Current Block" << endl;

    // for (int i = 0; i < B->Transactions.size(); i++)
    // {
    //     cout << B->Transactions[i] << endl;
    // }
    // Code For adding Block To BlockChain
    //  this->Blockchain.insert({B->blockId, B});
    //  lastBlockId++;
    // cout << "From Generate Block : ";
    // cout << "BlockChainLength " << this->blockChainLength << " | BlockLevel  : " << B->blockLevel << endl;

    Simulator->EventQueue.push(new Event(B, Simulator->globalTime + Simulator->prop_delay + this->RandomInterArrivalBlockTime(Simulator->blockInterArrivalMeanTime), "MineBlock", this, NULL));
    delete B;
}

void Node::MineBlock(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter)
{
    // cout << "BlockChainLength " << this->blockChainLength << " | BlockLevel  : " << currEvent->B.blockLevel << endl;
    if (this->blockChainLength + 1 != currEvent->B.blockLevel) // because the block was supposed to mine at the nextLevel
        return;
    string coinbaseMessage = to_string(this->NodeId) + " Mines 50 BTC";
    Transaction T(coinbaseMessage, currEvent->eventTime);
    currEvent->B.Transactions.push_back(T);
    currEvent->B.NodeBalances[this->NodeId] += 50;

    this->Blockchain.insert({currEvent->B.blockId, currEvent->B});
    if (currEvent->B.blockLevel == this->blockChainLength + 1)
    {
        this->blockChainLength++;
        this->lastBlockId = currEvent->B.blockId;
    }
    // after block is mined add an coinbase txn + 50 coins to the miner
    //  cout << "Yahan To aa Hi Gaya" << endl;
    this->BroadcastBlock(Simulator, currEvent);
    this->GenerateBlock(Simulator, BlockCounter);
}

void Node ::BroadcastBlock(DiscreteEventSimulator *Simulator, Event *currEvent)
{
    for (int i = 0; i < this->connectedPeers.size(); i++)
    {
        // cout << "YahinHoonMain" << endl;
        if (this->connectedPeers[i]->NodeId != currEvent->senderId) // LoopLess Forwarding
            Simulator->EventQueue.push(new Event(currEvent->B, Simulator->globalTime + Simulator->prop_delay, "ReceiveBlock", this, this->connectedPeers[i]));
    }
}

void Node ::ReceiveBlock(DiscreteEventSimulator *Simulator, Event *currEvent, int *BlockCounter)
{
    // cout << "Bas Yahi Kar Sakta Hun Mai abhi" << endl;
    bool flag;

    if (this->ReceivedBlocks.find(currEvent->B.blockId) != this->ReceivedBlocks.end()) // i.e Block already Received
    {
        this->GenerateBlock(Simulator, BlockCounter);
        return;
    }
    this->BlockArrivalTimes[currEvent->B.blockId] = currEvent->eventTime;

    this->ReceivedBlocks[currEvent->B.blockId] = true;

    int parentHash = currEvent->B.PrevHash;
    // vector<float> tempNodeBalance = this->Blockchain[parentHash].NodeBalances;

    // int count = currEvent->B.Transactions.size();

    if (this->Blockchain.find(parentHash) != this->Blockchain.end()) // Parent is found in blockchain
    {
        flag = this->VerifyAddBlock(currEvent->B); // Parent Found so verify & add Block
        if (flag)
            this->BroadcastBlock(Simulator, currEvent);
        // The Block was verified and added successfully
        Block TempBlock = currEvent->B;
        while (flag && (this->PendingBlocks.find(TempBlock.blockId) != this->PendingBlocks.end()))
        {
            flag = this->VerifyAddBlock(this->PendingBlocks[TempBlock.blockId]);
            if (flag)
                this->BroadcastBlock(Simulator, currEvent);
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

int Node::getMinedInLongestChain()
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

int Node ::getTotalMinedBlocks()
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