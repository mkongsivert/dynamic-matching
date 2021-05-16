import numpy as np
import math
import random
import sys
import matplotlib.pyplot as plt
from tqdm import tqdm

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
        return np.exp(-1*self._delta*self._elapsed_time) if self._matched else 0

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
        self._unmatched = [0,0]
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

    def loss(self):
        return self._unmatched[0] / self._unmatched[1] if self._unmatched[1] != 0 else 0

    def add_agent(self, a):
        new_node = [a]
        for node in self._compat_graph:
            #probability of compatibility = d/m
            if (random.randint(0,self._m) < self._d):
                new_node += [node[0]] # add connection
                node += [a]
        self._compat_graph+=[new_node]
        self._unmatched[0] += 1
        self._unmatched[1] += 1

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
                    self._unmatched[0] -= 2
                    return True
                else:
                    return False
        # if it gets to this point, the agent somehow is not in the market
        print("Error: Specified agent was not in the graph")
        return False

    def time_step(self):
        # get the number of agents to add from the poisson distribution
        new_agents = np.random.poisson(self._m, 1)[0]
        for i in range(new_agents):
            lifespan = np.random.poisson(self._lambda, 1)[0]
            new_a = Agent(lifespan, self._delta)
            self.add_agent(new_a)
            if (self._greedy):
                self.try_match(new_a)

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

def main():
    T = 2000
    lamb = 4
    m = 10
    d = 5
    delta = 0
    greedy = True

    data_greedy = []
    for i in range(2,11):
        all_losses = []
        print('Running trial: d = ' + str(i))
        for j in tqdm(range(50)):
            market = Market(lamb, m, i, delta, True)
            for k in range(T):
                market.time_step()
            all_losses += [market.loss()]
        avg_loss = np.mean(all_losses)
        data_greedy += [(1/(2*i+1), avg_loss, math.log(2)/i)]    
        # (theoretical) lower bound, (theoretical) upper bound, average over 50 trials

    data_patient = []
    for i in range(2,11):
        all_losses = []
        print('Running trial: d = ' + str(i))
        for j in tqdm(range(50)):
            market = Market(lamb, m, i, delta, False)
            for k in range(T):
                market.time_step()
            all_losses += [market.loss()]
        avg_loss = np.mean(all_losses)
        data_patient += [(np.exp(-1*i)/(i+1), avg_loss, np.exp(-1*i/2)/2)]    
        # (theoretical) lower bound, (theoretical) upper bound, average over 50 trials

    #plot data
    for i in range(2):
        data = (data_greedy if i == 0 else data_patient)
        fig = plt.figure()
        ax1 = fig.add_subplot()
        ax1.set_ylabel('Loss')
        ax1.set_xlabel('d')
        strategy = '(Greedy)' if i == 0 else '(Patient)'
        ax1.set_title('Effect of d on Loss '+ strategy)
        x_values = range(2,11)
        lower = [x[0] for x in data]
        avg = [x[1] for x in data]
        upper = [x[2] for x in data]
        plt.plot(x_values, lower, label='Lower bound', color='darkgreen')
        plt.plot(x_values, avg, label='Average loss', color='navy')
        plt.plot(x_values, upper, label='Upper bound', color='firebrick')
        plt.legend()
        plt.show()


if __name__ == "__main__":
    main()