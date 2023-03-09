from graphviz import Graph
from queue import Queue
import sys

class Block:
    def __init__(self, prev_block_hash):
        self.prev_block_hash = prev_block_hash

class BlockChain:
    def __init__(self, file_name):
        self.block_tree = {}
        with open(file_name, 'r') as f:
            for line in f:
                parts = line.strip().split()
                block_id, prev_block_hash = int(parts[0]), int(parts[1]) if parts[1] != 'None' else None
                self.block_tree[block_id] = [Block(prev_block_hash)]
        self.id = 123

    def visualize(self):
        tree = {}
        for block_id in self.block_tree.keys():
            parent_hash = self.block_tree[block_id][0].prev_block_hash
            if parent_hash not in tree.keys():
                tree[parent_hash] = {}
            tree[parent_hash][block_id] = self.block_tree[block_id][0]
        queue = Queue()
        queue.put(0)
        graph = Graph('parent', filename=str(self.id))
        graph.attr(rankdir='LR', splines='line')
        hash = {}
        while not queue.empty():
            size = queue.qsize()
            temp = Graph('child')
            while size>0:
                parent_hash = queue.get()
                for child_id in tree[parent_hash].keys():
                    temp.node(str(child_id), str(child_id)) # Modified line to set the label as block ID
                    hash[child_id] = str(child_id)
                    if parent_hash != 0:
                        graph.edge(str(child_id), hash[tree[parent_hash][child_id].prev_block_hash])
                    if child_id in tree.keys():
                        queue.put(child_id)
                size -= 1
            graph.subgraph(temp)
        graph.render('results/' + str(self.id) + '.png', view=True)

your_instance = BlockChain(sys.argv[1])
your_instance.visualize()
