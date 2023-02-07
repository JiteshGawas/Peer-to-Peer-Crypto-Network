// #include<iostream>
// #include<vector>
// #include<ctime>
// #include<time.h>
// #include<vector>
// #include <bits/stdc++.h>


// using namespace std;
// const int num_peers = 20;

// int adj[num_peers][num_peers];
// int visited[num_peers];
// int min_degree = 4;
// int max_degree = 8;

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


    for(int i =0; i<num_peers; i++)
    {   
        cout<<i<<"th peer started"<<"           No of edges here are ";
        int numConnections = min_degree + rand()%(max_degree - min_degree + 1);             //Choose between 4 and 8 (Logic in Book)
        //vector<int> neighbors;
        cout <<numConnections<<endl;
        int count=0;
        for(int a = 0; a<num_peers; a++)
        {
                    if(adj[i][a]==1) count++;
        }

        numConnections-=count;
        

        while(numConnections)
        {
            int k = rand()%(num_peers);
            if(i<k) 
            { adj[i][k] = 1;  adj[k][i] = 1 ; numConnections--;}

            else if(i>k){
                int count2=0;
                for(int a = 0; a<num_peers; a++)
                {
                    if(adj[k][a]==1) count2++;
                }

                if(count2<max_degree) { adj[i][k] = 1; adj[k][i] = 1; numConnections--;}
            }
        }
    
    cout<<"---------------------------------------------------------------------------------------------------------"<<endl;
    for(int i=0; i<num_peers; i++)
    {
        for(int j=0; j<num_peers; j++)
        {
            cout<<adj[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"---------------------------------------------------------------------------------------------------------"<<endl;

    }

    cout<<"GraphCreated"<<endl;
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