#include<iostream>
#include<vector>
#include<ctime>
#include<time.h>
#include<vector>
#include <bits/stdc++.h>


using namespace std;



int min_degree = 4;
int max_degree = 8;

void createGraph(const int num_peers)
{   


    cout<<"Graph Creation Started"<<endl;
    for(int i =0; i<num_peers; i++)
    {
        for(int j=0; j<num_peers; j++)
        {
            adj[i][j]= 0; // Initialize with all 0's
        }
    }


    for(int i =0; i<num_peers; i++)
    {
        cout<<i<<"th peer started";
        int numConnections = min_degree + rand()%(max_degree - min_degree + 1);             //Choose between 4 and 8 (Logic in Book)
        cout <<numConnections<<endl;
        vector<int> connectedPeers;
        for (int j = 0; j < numConnections; j++) {
            int v = rand() % num_peers + 1; 
            if (v == i || find(connectedPeers.begin(), connectedPeers.end(), v) != connectedPeers.end()) {
                j--;                // reduce j if the peer is already connected or connecting to itself
                continue;
            }
            connectedPeers.push_back(v);
            adj[i][v] = 1; // add the node to matrix
        }

    }
}



void dfs(int v) {
    int i;
    visited[v] = 1;
    for (i = 0; i <num_peers; i++)
        if (adj[v][i] && !visited[i]) {

            dfs(i);
        }
}

bool isConnected()
{
    cout<<"Checking Connectivity"<<endl;
    dfs(0);
    int count=0,i,j;
    for (i = 0; i < num_peers; i++) {
        if (visited[i])
            count++;
    }
    // cout<<count<<endl;
    if (count == num_peers)
        return true;
    else
        return false;

}


// int main()
// {
//     srand(time(0));
//     createGraph();
//     while(!isConnected())
//     {
//         createGraph();
//     }
//     cout<<"Created Graph"<<endl;


//     for(int i=0; i<num_peers; i++)
//     {
//         for(int j=0; j<num_peers; j++)
//         {
//             cout<<adj[i][j]<<" ";
//         }
//         cout<<endl;
//     }
// }