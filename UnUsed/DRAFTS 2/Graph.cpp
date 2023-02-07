#include <iostream>
#include "Graph.h"
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

void Graph ::createGraph()
{
    cout << "Graph Creation Started" << endl;
    for (int i = 0; i < this->numNodes; i++)
    {
        for (int j = 0; j < this->numNodes; j++)
        {
            adjMatrix[i][j] = 0; // Initialize with all 0's
        }
    }

    srand(time(0));

    for (int i = 0; i < this->numNodes; i++)
    {

        cout << i << "th peer started"
             << "           No of edges here are ";

        int edges = minDegree + (rand() % (maxDegree - minDegree + 1));

        cout << edges << endl;
        while (edges > 0)
        {
            int j = rand() % this->numNodes;
            if (j != i && adjMatrix[i][j] == 0)
            {
                adjMatrix[i][j] = 1;
                adjMatrix[j][i] = 1;
                edges--;
            }
        }
    }
}

bool Graph ::isConnected()
{
    cout << "Checking Connectivity" << endl;
    dfs(0);
    int count = 0, i, j;
    for (i = 0; i < this->numNodes; i++)
    {
        if (this->visited[i])
            count++;
    }
    // cout<<count<<endl;
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