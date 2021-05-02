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

Market::Market() :
    m_{0},
    d_{0},
    delta_{0},
    utility_total_{0}
{
    // Nothing left to do
}

Market::Market(uint64_t lambda, uint64_t m, uint64_t d, uint64_t delta) :
    m_{m},
    d_{d},
    delta_{delta},
    utility_total_{0}
{
    lifespan_dist_ = std::poisson_distribution<int>(lambda);
    new_agent_dist_ = std::poisson_distribution<int>(m);
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator_ = std::default_random_engine(seed);
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

float Market::time_step()
{
    float utility_increase = 0;
    for (auto itr = compat_graph_.begin(); itr != compat_graph_.end(); ++itr)
    {
        Agent* a = itr->front();
        if (a->time_step())
        {
            remove_agent(a); // remove critical agents
        }
    }
    //TODO: add a matching function

    // get the number of agents to add from the poisson distribution
    uint64_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    uint64_t new_agents = new_agent_dist_(generator_);
    for (uint64_t i = 0; i < new_agents; ++i)
    {
        uint64_t lifespan = lifespan_dist_(generator_);
        Agent new_a(lifespan, delta_);
        add_agent(&new_a);
    }

    return utility_increase;
}

int main()
{
    // Constants
    uint64_t T = 10;
    uint64_t lambda = 1;
    uint64_t m = 1;
    uint64_t d = 1;
    uint64_t delta = 5;

    Market market(lambda, m, d, delta);
    for (uint64_t i = 0; i < T; ++i)
    {
        std::cout << market.time_step() << std::endl;
    }
    return 0;
}