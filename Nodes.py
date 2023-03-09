import networkx as nx
import matplotlib.pyplot as plt
import numpy as np
import json
import sys
def draw_graph(adj_matrix, slow_nodes):
    G = nx.from_numpy_array(adj_matrix)
    pos = nx.spring_layout(G)

    node_colors = ['red' if i in slow_nodes else 'blue' for i in range(len(G))]
    edge_colors = ['red' if u in slow_nodes or v in slow_nodes else 'black' for u, v in G.edges()]

    nx.draw_networkx_nodes(G, pos, node_color=node_colors)
    nx.draw_networkx_labels(G, pos)
    nx.draw_networkx_edges(G, pos, edge_color=edge_colors)

    plt.show()


file_name = sys.argv[1]
with open(file_name, 'r') as f:
    count = 0
    for line in f:
        if(count==0):
            adj=line.strip().split()
            res = adj[0]

            adj_matrix = json.loads(res)
            adj_matrix = np.array(adj_matrix)

            count=count+1
        elif(count==1):

            slow_nodes=line.strip().split()
            slow_nodes = slow_nodes[0]

            slow_nodes = json.loads(slow_nodes)
            # print(type(slow_nodes))
            # print(slow_nodes)
            count+=1
        
# adj_matrix = np.array(adj)

# print(adj_matrix)
# print(type(adj_matrix))
# print(slow_nodes)

# print(len(adj_matrix))

# adj_matrix = np.array([
#     [0, 1, 0, 0, 1, 0, 1, 0, 0, 0],
#     [1, 0, 1, 0, 0, 0, 1, 0, 0, 0],
#     [0, 1, 0, 1, 0, 0, 0, 0, 0, 0],
#     [0, 0, 1, 0, 1, 0, 0, 0, 0, 0],
#     [1, 0, 0, 1, 0, 1, 1, 0, 0, 0],
#     [0, 0, 0, 0, 1, 0, 0, 0, 0, 0],
#     [1, 1, 0, 0, 1, 0, 0, 1, 1, 0],
#     [0, 0, 0, 0, 0, 0, 1, 0, 1, 0],
#     [0, 0, 0, 0, 0, 0, 1, 1, 0, 1],
#     [0, 0, 0, 0, 0, 0, 0, 0, 1, 0]
# # ])

# print(adj_matrix)
# slow_nodes = [1, 3, 5, 7]

draw_graph(adj_matrix, slow_nodes)
