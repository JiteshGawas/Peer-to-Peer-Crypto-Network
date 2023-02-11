#include "ClassDeclarations.h"
#include <iostream>
#include <random>
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
        PeerVec[i].balance = 30 + rand() % (21); // add some random numbers of BTC between 30-50 for each peer
        PeerVec[i].NWspeed = 1;
        PeerVec[i].CPU_Usage = 1;
        PeerVec[i].Blockchain.insert({1, new Block(1, 0)}); 
        PeerVec[i].lastBlockId = 1;

        // PeerVec[i].GenerateTransaction(Simulator, "Inits");
    }
    

    while (z0_Set.size() < (Simulator.z_0 * Simulator.numNodes))
    {
        z0_Set.insert(rand() % Simulator.numNodes); // cout<<"Z0 peers are"<<endl;
    }
    for (auto it = z0_Set.begin(); it != z0_Set.end(); ++it)
    {
        PeerVec[*it].NWspeed = 0;
    }

    while (z1_Set.size() < (Simulator.z_1 * Simulator.numNodes))
        z1_Set.insert(rand() % Simulator.numNodes);
    for (auto it = z1_Set.begin(); it != z1_Set.end(); ++it)
        PeerVec[*it].CPU_Usage = 0;

    for(int i =0; i<this->numNodes; i++)
    {
        
        for(int j=0; j<this->numNodes; j++)
        {   
            PeerVec[i].Blockchain[1]->NodeBalances.push_back(PeerVec[j].balance);
            
        }
        
    }
    
    
}

void Peers::PeerInfo()
{
    cout << "PeerId  Balance  NWSpeed  CPU_Usage" << endl;
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
        cout << PeerVec[i].NodeId << "  " << PeerVec[i].balance << "  " << PeerVec[i].NWspeed << "  " << PeerVec[i].CPU_Usage << endl;
        cout << "Genesis Block Balances For Node " << i<<endl;
        for(int j=0; j<this->numNodes; j++)
        {
            cout<<PeerVec[i].Blockchain[1]->NodeBalances[j]<<" ";
        }
        cout<<endl;

    }
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

void Node ::GenerateTransaction(DiscreteEventSimulator *Simulator, string TxnType)
{
    // srand(time(0));
    int coins = rand() % 60;
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
        receiverID = this->NodeId;
        ss << this->NodeId << " " << TxnType << " "
           << " " << coins << " BTC"; // A Inits
    }

    string Message = ss.str();

    // cout << Message << endl;
    Transaction *T;
    // cout << T << endl;
    if (TxnType == "Pays")
    {
        T = new Transaction(Message, Simulator->globalTime + Simulator->interArrivalTxnTime);
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


void Node ::GenerateBlock(DiscreteEventSimulator *Simulator)
{
    Block *B= new Block(this->lastBlockId+1, this->lastBlockId);
    int count=10;
    B->blockId = this->NodeId;
    cout<<"Block is Mined by the Node "<<B->blockId<<endl;
    B->NodeBalances = this->Blockchain[lastBlockId]->NodeBalances;
    // cout<<"I am here"<<endl;

    for (auto itr = this->PendingTransaction.begin(); itr != this->PendingTransaction.end(); ++itr) {
        if(B->NodeBalances[itr->second.senderId] > itr->second.coins) //Balance of sender > Txn sender coins
        {
            B->Transactions.push_back(itr->second);
            count--;
            B->NodeBalances[itr->second.senderId] -= itr->second.coins;
            B->NodeBalances[itr->second.receiverId] += itr->second.coins; 
            // this->PendingTransaction.erase(itr->first);

        }
        
        if(count == 0) break;
        
    }
    // cout<<"Working"<<endl;
    
    // for (auto i = B->NodeBalances.begin(); i != B->NodeBalances.end(); ++i)
    //     cout << *i << " ";

    for(int i =0; i<B->NodeBalances.size(); i++)
    {
        cout<<B->NodeBalances[i]<<" ";
    }

    cout<<endl;

}