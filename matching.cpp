/**
 * \file matching.cpp
 *
 * \authors Mackenzie Kong-Sivert
 *
 * \brief Implementation of the model described in Akbarpour 2014
 */

#include "matching.hpp"

#include <math.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <chrono>
#include <random>

Agent::Agent() :
    lifespan_{0},
    delta_{0},
    elapsed_time_{0},
    matched_{false}
{
    // Nothing to do here
}

Agent::Agent(uint64_t lifespan, float delta) :
    lifespan_{lifespan},
    delta_{delta},
    elapsed_time_{0},
    matched_{false}
{
    // Nothing left to do
}

bool Agent::time_step()
{
    ++elapsed_time_;
    return elapsed_time_==lifespan_;
}

float Agent::utility()
{
    return (matched_ ? exp(-1*delta_*elapsed_time_) : 0);
}

void Agent::match()
{
    matched_ = true;
}

Market::Market() :
    m_{0},
    d_{0},
    delta_{0},
    utility_total_{0}
{
    // Nothing left to do
}

Market::Market(uint64_t lambda, uint64_t m, uint64_t d, uint64_t delta, bool greedy) :
    m_{m},
    d_{d},
    delta_{delta},
    greedy_{greedy},
    utility_total_{0}
{
    lifespan_dist_ = std::poisson_distribution<int>(lambda);
    new_agent_dist_ = std::poisson_distribution<int>(m);
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator_ = std::default_random_engine(seed);
}

void Market::change_strategy(std::string strategy)
{
    if (strategy == "greedy")
    {
        greedy_ = true;
    }
    else if (strategy == "patient")
    {
        greedy_ = false;
    }
    else
    {
        greedy_ = !greedy_;
    }
}

float Market::utility()
{
    return utility_total_;
}

void Market::add_agent(Agent* a)
{
    std::list<Agent*> new_node {a};
    for (auto itr = compat_graph_.begin(); itr != compat_graph_.end(); ++itr)
    {
        if (rand()%m_ < d_) // probability of compatibility = d/m
        {
            new_node.push_back(itr->front()); // add connection
            itr->push_back(a);
        }
    }
    compat_graph_.push_back(new_node);
}

void Market::remove_agent(Agent* a)
{
    // find and remove the row for a
    auto a_itr = compat_graph_.end();
    for (auto itr = compat_graph_.begin(); itr != compat_graph_.end(); ++itr)
    {
        if (itr->front() == a)
        {
            a_itr = itr;
            break;
        }
    }
    if (a_itr != compat_graph_.end())
    {
        compat_graph_.erase(a_itr);
    }
    else
    {
        // throw an error or something
    }

    // find and remove a from all remaining rows
    for (auto itr1 = compat_graph_.begin(); itr1 != compat_graph_.end(); ++itr1)
    {
        auto a_itr = itr1->end();
        for (auto itr2 = itr1->begin(); itr2 != itr1->end(); ++itr2)
        {
            if (*itr2 == a)
            {
                a_itr = itr2;
                break;
            }
        }
        if (a_itr != itr1->end())
        {
            itr1->erase(a_itr);
        }
    }
}

bool Market::try_match(Agent* a)
{
    for (auto itr = compat_graph_.begin(); itr != compat_graph_.end(); ++itr)
    {
        if (*itr->begin() == a)
        {
            if (itr->size() > 1) // if there are any compatible agents, match one
            {
                auto b_itr = ++itr->begin();
                Agent* b = *b_itr;
                a->match();
                utility_total_ += a->utility();
                b->match();
                utility_total_ += b->utility();
                remove_agent(a);
                remove_agent(b);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    // if it gets to this point, the agent somehow is not in the market
    std::cout << "Error: Specified agent was not in the graph" << std::endl;
    return false;
}

void Market::time_step()
{
    std::list<Agent*> to_remove;
    for (auto itr = compat_graph_.begin(); itr != compat_graph_.end(); ++itr)
    {
        Agent* a = itr->front();
        if (a->time_step())
        {
            if (greedy_)
            {
                to_remove.push_back(a); // mark critical agents
            }
            else // under a patient strategy, try to match the agent at the end
            {
                bool matched = try_match(a);
                if (!matched)
                {
                    to_remove.push_back(a); // mark unmatched critical agents
                }
            }
        }
    }

    // erase all critical agents
    for (auto itr = to_remove.begin(); itr != to_remove.end(); ++itr)
    {
        remove_agent(*itr);
    }

    // get the number of agents to add from the poisson distribution
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    uint64_t new_agents = new_agent_dist_(generator_);
    for (uint64_t i = 0; i < new_agents; ++i)
    {
        uint64_t lifespan = lifespan_dist_(generator_);
        Agent* new_a = new Agent(lifespan, delta_);
        add_agent(new_a);
        if (greedy_)
        {
            try_match(new_a);
        }
    }
}

int main()
{
    // Constants
    uint64_t T = 20;
    uint64_t lambda = 4;
    uint64_t m = 10;
    uint64_t d = 5;
    uint64_t delta = 0;
    bool greedy = true;

    Market market(lambda, m, d, delta, greedy);
    for (uint64_t i = 0; i < T; ++i)
    {
        market.time_step();
        std::cout << "Total utility: " << market.utility() << std::endl;
    }
    return 0;
}