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
        Block B(1,0);
        PeerVec[i].Blockchain.insert({1, B}); 
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
            PeerVec[i].Blockchain[1].NodeBalances.push_back(PeerVec[j].balance);
            
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
            cout<<PeerVec[i].Blockchain[1].NodeBalances[j]<<" ";
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
    Block *B= new Block(this->lastBlockId+1, this->lastBlockId);   //Create new Block
    int count=10;    //10 transactions for check
    B->blockId = this->NodeId;
    cout<<"Block is Mined by the Node "<<B->blockId<<endl;   //Block is mined by Id 
    B->NodeBalances = this->Blockchain[lastBlockId].NodeBalances;
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
    
    this->BroadcastBlock(Simulator,B);

}

// m.find(ch) != m.end()

void Node ::ReceiveBlock(DiscreteEventSimulator *Simulator , Event *currEvent)
{
    int flag = 0;
    if( this->AllBlocks.find(currEvent->B.blockId) != this->AllBlocks.end() )    //Block Id is allready present with the node 
    {
        return;
    }
    this->AllBlocks[currEvent->B.blockId] = true;    //If not present mark it present now

    int parentHash = currEvent->B.PrevHash;   //Calculate parent hash
    vector<float> tempNodeBalance = this->Blockchain[currEvent->B.PrevHash].NodeBalances; //wrong

    // int count = currEvent->B.Transactions.size();

    if( this->Blockchain.find(parentHash) != this->Blockchain.end())     //Parent is found in blockchain
    {
        // int count = currEvent->B.Transactions.size();

        for (int i = 0; i <currEvent->B.Transactions.size(); ++i)     //Check if all transactions are valid
        {
         //Balance of sender > Txn sender coins
        
                tempNodeBalance[currEvent->B.Transactions[i].senderId] -= currEvent->B.Transactions[i].coins;
                tempNodeBalance[currEvent->B.Transactions[i].receiverId] += currEvent->B.Transactions[i].coins;
                // this->PendingTransaction.erase(itr->first);
        }

        for(int i =0; i<tempNodeBalance.size(); i++)
        {
            if(tempNodeBalance[i] == currEvent->B.NodeBalances[i])
            {
                continue;
            }
            else{
                flag = 1;
                break;            
            }
        }
        if(flag ==0)    // Block is valid
        {
            this->Blockchain.insert({currEvent->B.blockId, currEvent->B});  // Added to Blockchain
            //Lenght++ to be added

              
        }
        //search if this newly added block is a parent of some block which are left in nonverifiedblocks
        if( this->pendingBlocks.find(currEvent->B.blockId) != this->pendingBlocks.end())
        {
            //verify and add this block to blockchain 
            tempNodeBalance = currEvent->B.NodeBalances;

            for (int i = 0; i < this->pendingBlocks[currEvent->B.blockId].Transactions.size() ; i++)     //Check if all transactions are valid
            {
            //Balance of sender > Txn sender coins
            
                    tempNodeBalance[this->pendingBlocks[currEvent->B.blockId].Transactions[i].senderId] -= this->pendingBlocks[currEvent->B.blockId].Transactions[i].coins;
                    tempNodeBalance[this->pendingBlocks[currEvent->B.blockId].Transactions[i].receiverId] += this->pendingBlocks[currEvent->B.blockId].Transactions[i].coins;
                    // this->PendingTransaction.erase(itr->first);
            }

            for(int i =0; i<tempNodeBalance.size(); i++)
            {
                if(tempNodeBalance[i] == this->pendingBlocks[currEvent->B.blockId].NodeBalances[i])
                {
                    continue;
                }
                else{
                    flag = 1;
                    break;            
                }
            }
            if(flag ==0)    // Block is valid
            {
                this->Blockchain.insert({this->pendingBlocks[currEvent->B.blockId].blockId , this->pendingBlocks[currEvent->B.blockId]});  // Added to Blockchain
                //Lenght++ to be added

                
            }

            
        }
    }

    else    // Parent not found in blockchain
    {
        this->pendingBlocks.insert({currEvent->B.PrevHash, currEvent->B});

    }
}


void Node ::BroadcastBlock(DiscreteEventSimulator *Simulator, Event *currEvent)
{
    for (int i = 0; i < this->connectedPeers.size(); i++)
    {
        Simulator->EventQueue.push(new Event(currEvent->B, Simulator->globalTime + Simulator->prop_delay, "ReceiveBlock", this, this->connectedPeers[i]));
    }
}

void Node ::BroadcastBlock(DiscreteEventSimulator *Simulator, Block *B)
{
    for (int i = 0; i < this->connectedPeers.size(); i++)
    {
        Simulator->EventQueue.push(new Event(B, Simulator->globalTime + Simulator->prop_delay, "ReceiveBlock", this, this->connectedPeers[i]));
    }
}