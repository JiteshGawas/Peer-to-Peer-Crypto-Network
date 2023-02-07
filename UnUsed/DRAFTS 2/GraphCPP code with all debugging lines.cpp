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
    vector<vector<int>> adjList;
    adjList.resize(this->numNodes);
    // cout << "Graph Creation Started" << endl;
    srand(time(0));

    for (int i = 0; i < (this->numNodes); i++)
    {
        // cout << i + 1 << "th peer started"
        //      << "           No of edges here are ";

        int edges = minDegree + (rand() % (maxDegree - minDegree + 1));

        // cout << edges << endl;

        // if (adjMatrix[i].size() <= this->maxDegree)
        edges = edges - adjList[i].size();
        // cout << "LeftOver Edges " << edges << endl;
        while (edges > 0)
        {
            int j = rand() % this->numNodes; // int j = rand() % this->numNodes;
            // while (j <= i)
            //     j = rand() % this->numNodes;

            if (adjMatrix[j].size() == 8)
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
        // for (int k = 0; k < adjList[i].size(); k++)
        //     cout << adjList[i][k] << " ";

        // cout << "\nGenerated Size " << adjList[i].size() << "\n"
        //      << endl;

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