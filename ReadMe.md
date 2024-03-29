# Spring 2023 CS 765 - Project part II
## Simulating a selfish mining attack using the P2P Cryptocurrency Network developed in Project I.
### Under guidance of Prof Vinay Ribeiro at IIT Bombay CSE in the course CS 765 : Introduction of Blockchains, Cryptocurrency and Smart Contracts 


# Requirements
--Simulation:
	-C++
	-makefile

--Visualization of Blockchain
	-numpy
	-matplotlib
	-pandas
	-networkx
	
## Compilation of Code:
-To compile the simulator, simply run the make file.
-Incase you do not want to run the makeile, here is how you can run the simulator:
	g++ main.cpp Graph.cpp Simulator.cpp Peers.cpp transactions.cpp Blockchain.cpp Event.cpp -o main.cpp Graph.cpp Simulator.cpp Peers.cpp transactions.cpp Blockchain.cpp Event.cpp -lstdc++fs

## Run the code:
-You can run the code by the command ./Build/main z0 z1 AdvHashingPower %advConnectedTo type("self"/"stub") > outputfile.txt
-Here z0 and z1 are the 2 command line parameters that show the percentage of slow nodes and the percentage of CPU usage of nodes. AdvHashingPower is the percentage of hashing power (can be tried as 0.3,0.5 etc). %advConnectedTo is the percentage of nodes adversary is connected to in the graph. self is selfishMining and stub is Stubborn Mining.
outputfile.txt is the file where you write the final result.

## Running the Visualizations :
-You have to go to the folder Visualize
-There are 3 visualizations. 1)Graph of the network   2)Blockchain of the network   3) Node wise analysis of the blockchain
-You need to run this 3 files by giving the command line arguments as the input files which we get as output from the simulator

## Output:
-The output of the simulator will be stored in the logs folder.
-The log folder consists of Blockchain and other folder of BlockArrival Times that is asked in Part 8.
-The ratios asked are stored in the file named Params_Ratios.txt.
-The simulator file stores a pdf of the blockchain created.




## Description of the Problem Statement:
-The problem statement is about building a discrete-event simulator for a peer-to-peer (P2P) cryptocurrency network. 

-The cryptocurrency network is made up of several nodes, each with a unique ID. These nodes are randomly classified as "fast" or "slow" and "high CPU" or "low CPU". Each node generates transactions randomly over time, with the time between transactions following an exponential distribution. Each transaction consists of a "TxnID" which is the unique ID of the transaction, "IDx" which is the ID of the node generating the transaction, "IDy" which is the ID of the node receiving the transaction, and "C" which is the amount of cryptocurrency being transferred. The simulator ensures that the amount of cryptocurrency being transferred is less than or equal to the number of coins owned by the sender node.

-The nodes are also randomly connected to between 4 and 8 other nodes, forming a connected graph. The simulator simulates the latencies between each pair of nodes, which is the time taken for a message to be transmitted from one node to another. The latency between two nodes is determined by the link speed between them, the length of the message being transmitted, and the queuing delay at the node forwarding the message. The simulator randomly chooses the queuing delay from an exponential distribution, with a mean inversely related to the link speed.

-The nodes in the network also forward transactions they hear from one node to another connected node, provided they haven't already sent the same transaction to that node or haven't heard the transaction from that node before. The transactions form a chain of blocks, with each block containing several transactions. Each block must have a unique ID and is mined using a Proof-of-Work (PoW) mechanism. When a node receives a block from another node, it validates all its transactions, and if the block is valid, it adds the block to its chain.

-To mine a block, a node selects a subset of the transactions it has received and not included in any blocks in the longest chain. The node generates a random variable to determine the time it will take to mine the next block. The inter-arrival time between blocks is determined by an exponential distribution with a mean equal to a fixed time, such as 600 seconds in Bitcoin. If the node is a high CPU node, it has 10 times the hashing power of a low CPU node. The node that mines a block with a new longest chain broadcasts the block to all other nodes in the network, and the other nodes add it to their chains if it is valid. The result is a blockchain - a distributed and immutable ledger of transactions that is updated in a decentralized manner.

-Simulate Selfish Mining Attack : The Selfish Mining Attack, originally proposed by Eyal and Sirer in the paper "Majority is not Enough," is a strategy used by a selfish miner (adversary) to intentionally fork the chain by keeping their discovered blocks private. While honest nodes continue to mine on the public chain, the adversary mines on their own private branch. If the adversary mines more blocks, it develops a longer lead over the public chain and continues to keep these new blocks private. When the public branch approaches the adversary's private branch in length, it reveals blocks from its private chain to the public. The attacker follows a set of steps, which are similar to those discussed in the paper by Eyal. However, the steps followed by the attacker in this paper are slightly different. It is important to note that the attacker does not forward blocks generated by other nodes. A detailed explanation of the attack can be found in the "Majority is not Enough" paper by Gun Sirer et al.

-Simulate Stubborn mining attack : The Stubborn Mining Attack is a variation of the selfish mining attack, where instead of immediately revealing their entire private chain to guarantee that the network chooses their private chain over the public chain, the adversary only reveals the next block on their private chain to match the length of the public chain. If the lead of the selfish miner is 2 and an honest node finds the next block and closes the gap by 1, the state transitions to lead = 0 in selfish mining. However, in stubborn mining, the state transitions to lead = 1 instead. The adversary risks losing their private chain if the honest nodes succeed in advancing the public chain. On the other hand, if the adversary or a fraction of honest nodes successfully advances the adversary's private fork, they have successfully diverted a part of honest nodes to do useless work. Note that the parameter γ, used in the paper for analysis purposes, may vary depending on the network topology and the speed of nodes in your simulator. If the adversary wins a race in state lead = 0, the selfish miner reveals their new block to the public, transitioning to lead = 0. In contrast, the stubborn miner conceals their new block and continues mining on it privately, proceeding to state lead = 1

