#include<iostream>
#include<vector>
#include<ctime>
#include<time.h>
#include<vector>

using namespace std;

const int num_peers = 40;
int min_degree = 4;
int max_degree = 8;


int adj[num_peers][num_peers];
vector<int> peers[num_peers];

int visited[num_peers];



void createGraph()
{
    cout<<"Graph Creation Started"<<endl;
    for(int i =0; i<num_peers; i++)
    {
        for(int j=0; j<num_peers; j++)
        {
            adj[i][j]= 0; // Initialize with all 0's
        }
    }

    srand(time(0));
    cout<<"Graph initialized"<<endl;

    for(int i =0; i<num_peers; i++)
    {
        cout<<i<<"th peer started";
        int edges = min_degree + rand()%(max_degree - min_degree + 1); //Choose between 4 and 8
        cout << edges<<endl;
        while(edges>0)  // Add those edges in symmetry 
        {
            // cout<<"Adding edges in "<<i<<endl;
            int j = rand()%num_peers;
            // while(j<=i)
            // {
                // int j = rand()%num_peers;
            // }
            if( i<j && adj[i][j]==0)
            {
                adj[i][j] = adj[j][i] = 1;
                peers[i].push_back(j);
                peers[j].push_back(i);
                edges--;

            }

        }
    }
    cout<<"Graph Created"<<endl;

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


int main()
{
    createGraph();
    while(!isConnected())
    {
        createGraph();
    }
    cout<<"Created Graph"<<endl;


    for(int i=0; i<num_peers; i++)
    {
        for(int j=0; j<num_peers; j++)
        {
            cout<<adj[i][j]<<" ";
        }
        cout<<endl;
    }
}