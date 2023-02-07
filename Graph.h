#pragma once
#include <vector>
using namespace std;

class Graph
{
public:
    vector<vector<int>> adjMatrix;
    vector<int> visited;
    int numNodes, minDegree, maxDegree;

    Graph(int numNodes, int minDegree, int maxDegree);
    void createGraph();
    bool isConnected();
    void dfs(int v);
    bool does_exist(const vector<vector<int>> &adjList, int row, int item);
    friend class Peer;
};