#include "ClassDeclarations.h"
#include <iostream>
#include <vector>
using namespace std;

Graph ::Graph(int numNodes, int minDegree, int maxDegree)
{
    this->numNodes = numNodes;
    this->minDegree = minDegree;
    this->maxDegree = maxDegree;
    adjMatrix.resize(this->numNodes);
    visited.resize(this->numNodes);

    for (int i = 0; i < this->numNodes; i++)
    {
        adjMatrix[i].resize(this->numNodes);
    }
}

void Graph ::createGraph(float advMinPow, int numHonest) // Create graph using adjacency list such that each node is connected to 4 to 8 peers.
{
    vector<vector<int>> adjList;
    adjList.resize(this->numNodes);

    srand(time(0));
    int edges, j;

    for (int i = 0; i < (this->numNodes); i++)
    {
        if (i == 0)
        {
            edges = advMinPow * numHonest;
        }
        else
        {
            edges = minDegree + (rand() % (maxDegree - minDegree + 1));
        }

        edges = edges - adjList[i].size();
        while (edges > 0)
        {
            j = rand() % this->numNodes;
            while (j == 0)
            {
                j = rand() % this->numNodes; // To generate random numbers in order to connect to a peer
            }

            if (adjList[j].size() == 8 && i != 0)
            {
                continue;
            }

            if (j != i && !does_exist(adjList, j, i))
            {
                adjList[i].push_back(j);
                adjList[j].push_back(i);
                edges--;
            }
        }

        for (int i = 0; i < this->numNodes; i++)
        {
            for (int j = 0; j < this->numNodes; j++)
            {
                adjMatrix[i][j] = 0; // Initialize with all 0's
            }
        }

        for (int i = 0; i < adjList.size(); i++)
        {
            for (int j = 0; j < adjList[i].size(); j++)
            {
                adjMatrix[i][adjList[i][j]] = 1; // Make Connected Peers 1
            }
        }
    }
}

bool Graph ::isConnected() // Check if graph is connected
{
    dfs(0);
    int count = 0, i, j;
    for (i = 0; i < this->numNodes; i++)
    {
        if (this->visited[i])
            count++;
    }
    if (count == this->numNodes)
        return true;
    else
        return false;
}

void Graph::dfs(int v)
{
    int i;
    this->visited[v] = 1;
    for (i = 0; i < this->numNodes; i++)
        if (this->adjMatrix[v][i] && !this->visited[i])
        {

            this->dfs(i);
        }
}

bool Graph::does_exist(const vector<vector<int>> &adjList, int row, int item)
{
    int flag = 0;

    for (int i = 0; i < adjList[row].size(); i++)
    {
        if (adjList[row][i] == item)
            flag = 1;
    }

    if (flag)
        return true;
    else
        return false;
}