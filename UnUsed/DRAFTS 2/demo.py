class DiscreteEventSimulator:
    def __init__(self,a,b,c,d):
        self.num_peers = a
        self.z0 = b
        self.z1 = c
        self.interArrivalTime = d

    def printParams(self):
        print(self.num_peers, self.z0, self.z1, self.interArrivalTime)


simulator = DiscreteEventSimulator(50, 0.2, 0.4, 20)
simulator.printParams()