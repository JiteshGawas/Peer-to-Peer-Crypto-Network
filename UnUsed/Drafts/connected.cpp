#include <iostream>
#include <vector>

using namespace std;

void DFS(vector<vector<int>>& adj, int u, vector<bool>& visited) {
    visited[u] = 1;
    for (int v = 0; v < adj.size(); v++) {
        if (adj[u][v] == 1 && !visited[v]) {
            DFS(adj, v, visited);
        }
    }
}

bool isConnected(vector<vector<int>>& adj) {
    int V = adj.size();
    vector<bool> visited(V, 0);
    int i;
    for (int i = 0; i < V; i++) {
        int sum = 0;
        for (int j = 0; j < V; j++) {
            sum += adj[i][j];
        }
        if (sum > 0) {
            break;
        }
    }

    DFS(adj, i, visited);

    for (int i = 0; i < V; i++) {
        int sum = 0;
        for (int j = 0; j < V; j++) {
            sum += adj[i][j];
        }
        if (!visited[i] && sum > 0) {
            return false;
        }
    }

    return true;
}

int main() {
    vector<vector<int>> adj;

    adj={{0,1,1,0}, {1,0,1,0}, {1,1,0,0}, {0,0,0,0}};

    if (isConnected(adj)) {
        cout << "The graph is connected." << endl;
    } else {
        cout << "The graph is not connected." << endl;
    }

    return 0;
}
