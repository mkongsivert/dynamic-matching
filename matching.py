import numpy
import random
from scipy.stats import poisson
import sys

class Agent:
    def __init__(self, lifespan, delta):
        self._lifespan = lifespan
        self._delta = delta
        self._elapsed_time = 0
        self._matched = False

    def time_step(self):
        self._elapsed_time +=1
        return (self._elapsed_time == self._lifespan)

    def utility(self):
        return numpy.exp(-1*self._delta*self._elapsed_time) if self._matched else 0

    def match(self):
        self._matched = True

class Market:
    def __init__(self, lamb, m, d, delta, greedy) :
        self._m = m
        self._d = d
        self._delta = delta
        self._lambda = lamb
        self._greedy = greedy
        self._utility_total = 0
        self._compat_graph = []

    def change_strategy(self, strategy):
        if (strategy == "greedy"):
            self._greedy = true
        elif (strategy == "patient"):
            self._greedy = false
        else:
            self._greedy = not self._greedy

    def utility(self):
        return self._utility_total

    def add_agent(self, a):
        new_node = [a]
        for node in self._compat_graph:
            #probability of compatibility = d/m
            if (random.randint(0,self._m) < self._d):
                new_node += [node[0]] # add connection
                node += [a]
        self._compat_graph+=[new_node]

    def remove_agent(self, a):
        # find and remove the row for a
        a_ind = sys.maxsize
        for i in range(len(self._compat_graph)):
            if (self._compat_graph[i][0] == a):
                a_ind = i
                break
        if a_ind != sys.maxsize:
            del self._compat_graph[a_ind]
        else:
            print("Error: Specified agent was not in the graph")
            return

        # find and remove a from all remaining rows
        for node in self._compat_graph: 
            if a in node:
                node.remove(a)

    def try_match(self, a):
        for node in self._compat_graph:
            if (node[0] == a):
                if (len(node) > 1): # if there are any compatible agents, match one
                    b = node[1]
                    a.match()
                    self._utility_total += a.utility()
                    b.match()
                    self._utility_total += b.utility()
                    self.remove_agent(a)
                    self.remove_agent(b)
                    return True
                else:
                    return False
        # if it gets to this point, the agent somehow is not in the market
        print("Error: Specified agent was not in the graph")
        return False

    def time_step(self):
        to_remove = []
        for node in self._compat_graph:
            a = node[0]
            if (a.time_step()):
                if (self._greedy):
                    to_remove+=[a] # mark critical agents
                else: # under a patient strategy, try to match the agent at the end
                    matched = self.try_match(a)
                    if (not matched):
                        to_remove+=[a] # mark unmatched critical agents

        # erase all critical agents
        for a in to_remove:
            self.remove_agent(a)

        # get the number of agents to add from the poisson distribution
        new_agents = numpy.random.poisson(self._m, 1)[0]
        for i in range(new_agents):
            lifespan = numpy.random.poisson(self._lambda, 1)[0]
            new_a = Agent(lifespan, self._delta)
            self.add_agent(new_a)
            if (self._greedy):
                self.try_match(new_a)

def main():
    T = 20
    lamb = 4
    m = 10
    d = 5
    delta = 0
    greedy = True

    market = Market(lamb, m, d, delta, greedy)
    for i in range(T):
        market.time_step()
        print("Total utility: " + str(market.utility()))

if __name__ == "__main__":
    main()