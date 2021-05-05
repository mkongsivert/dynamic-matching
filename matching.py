import numpy
from scipy.stats import poisson

class Agent:
    def __init__(self, lifespan, delta):
        self._lifespan = lifespan
        self._delta = delta
        self._elapsed_time = 0
        self._matched = False

    def time_step(self):
        _elapsed_time +=1
        return (_elapsed_time == _lifespan)

    def utility(self):
        return numpy.exp(-1*self._delta*self._elapsed_time) if self._matched else 0

    def match(self):
        self._matched = True

class Market:
    def __init__(self, lamb, m, d, delta, greedy) :
        self._m = m
        self._d = d
        self._delta = delta
        self._greedy = greedy
        self._utility_total = 0
        self._lifespan_dist = poisson.ppf()
        #lifespan_dist_ = std::poisson_distribution<int>(lambda);
        #new_agent_dist_ = std::poisson_distribution<int>(m);