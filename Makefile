MAIN=Build/main

$(MAIN) : main.cpp Graph.cpp Simulator.cpp Peers.cpp transactions.cpp Blockchain.cpp Event.cpp
	g++ main.cpp Graph.cpp Simulator.cpp Peers.cpp transactions.cpp Blockchain.cpp Event.cpp -o $(MAIN) -lstdc++fs
