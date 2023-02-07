#include<iostream>
#include<chrono>
#include<random>
#include<set>
#include<vector>
#include<time.h>

using namespace std;


#define MAX_PEERS 10
int min_degree = 4;
int max_degree = 8;
int adj[MAX_PEERS][MAX_PEERS];
int visited[MAX_PEERS];


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

void dfs(int v, int num_peers) {
    int i;
    visited[v] = 1;
    for (i = 0; i <num_peers; i++)
        if (adj[v][i] && !visited[i]) {

            dfs(i,num_peers);
        }
}

bool isConnected(int num_peers)
{
    cout<<"Checking Connectivity"<<endl;
    dfs(0,num_peers);
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



class DiscreteEventSimulator{
    int num_peers;
    float z_0, z_1;
    int interArrivalTime;
    // int peerConnectedGraph[MAX_PEERS][MAX_PEERS] //or can be created dynamically after we get the "N" from command line
    // int EventQueue[100];

    public :
    
    DiscreteEventSimulator(int a, float b, float c, float d){
        num_peers = a;
        z_0 = b;
        z_1 = c;
        interArrivalTime = d;
    }

    void PrintParameters()
    {
        cout<<num_peers<<" "<<z_0<<" "<<z_1<<" "<<interArrivalTime<<endl;
    }

    int getNumPeer(){
        return num_peers;
    }

    float get_Z0(){
        return z_0;
    }
    
    float get_Z1(){
        return z_1;
    }

    int get_interArrivalTime(){
        return interArrivalTime;
    }
};

class Transaction{
public:
int txnID, senderID, ReceiverID, coins;

};

class Peer{
public:    
int peerId,  NWspeed, CPU_Usage;
float balance; 
// NWspeed : 0 (Slow) 1(Fast) | CPU_Usage = 0 (low_cpu) 1(high_cpu)

};





int main()
{   

    int seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator (seed);
    exponential_distribution<float> InterArrivalMeanTime (30);

    uniform_int_distribution<int> Uni_sol(10,500); // for speed of light between 10 ms to 500 ms
    
    int prop_delay = Uni_sol(generator);

    vector <Peer> Peers;
    set <int> z0_Set,z1_Set;

    DiscreteEventSimulator Simulator(MAX_PEERS, 0.2, 0.4, InterArrivalMeanTime(generator));
    Simulator.PrintParameters();

    Peers.resize(Simulator.getNumPeer());

    for(int i=0;i<Simulator.getNumPeer();i++)
    {
        Peers[i].peerId = i;
        Peers[i].balance = i; //add some random numbers of BTC
        Peers[i].NWspeed = 1;
        Peers[i].CPU_Usage = 1;
    }

    srand(time(0));
    while(z0_Set.size() < (Simulator.get_Z0() * Simulator.getNumPeer()))  z0_Set.insert(rand() % Simulator.getNumPeer()); //cout<<"Z0 peers are"<<endl;
    for (auto it = z0_Set.begin(); it != z0_Set.end(); ++it) Peers[*it].NWspeed = 0;

    while(z1_Set.size()< (Simulator.get_Z1() * Simulator.getNumPeer()))    z1_Set.insert(rand() % Simulator.getNumPeer());
    for (auto it = z1_Set.begin(); it != z1_Set.end(); ++it) Peers[*it].CPU_Usage = 0;

    // cout<<"ReachedHere";
    

    cout<<"PeerId  Balance  NWSpeed  CPU_Usage"<<endl;


    for(int i=0;i<Simulator.getNumPeer();i++)
    {
        cout<<Peers[i].peerId<<"  "<<Peers[i].balance<<"  "<<Peers[i].NWspeed<<"  "<<Peers[i].CPU_Usage<<endl;
    }

//Creating Connected Graph Among Peers
    
    srand(time(0));
    createGraph(Simulator.getNumPeer());
    while(!isConnected(Simulator.getNumPeer())) createGraph(Simulator.getNumPeer());
    


}
