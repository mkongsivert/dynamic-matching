/**
 * -  For t >= 0, let At be the set of the agents in our market at time t, and let Zt := |At|. We refer to At as the pool of the market
 * - assume A_0 = \varnothing
 * - runs in interval [0,T]
 * - Agents arrive at the market at rate m according to a Poisson process, i.e., in any interval [t, t + 1], m new agents enter the market in expectation. Throughout the paper we assume m >= 1

*/

/**
 * \file matching.cpp
 *
 * \authors Mackenzie Kong-Sivert
 *
 * \brief Header for the model described in Akbarpour 2014
 */

#ifndef MATCHING_HPP_INCLUDED
#define MATCHING_HPP_INCLUDED 1

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <random>

/**
 * \class Agent
 * \brief Represents an agent in our market
 *
 * \details TODO
 *
 * \remarks TODO
 */
class Agent {
public:
    /**
     * \brief Default constructor
     *
     * \note will not be used; only exists for compilation
     */
    Agent();
    /**
     * \brief Parameterized constructor
     *
     * \note
     */
    Agent(uint64_t lifespan, float delta);

    /**
     * \brief increments elapsed time
     * 
     * \return true if the agent has become critical and false otherwise
     * 
     * \note
     */
    bool time_step();

    /**
     * \brief utility function
     * 
     * \return utility of the 
     * 
     * \note = e^(delta*(elapsed_time))
     */
    float utility();

private:
    uint64_t lifespan_;
    float delta_;
    uint64_t elapsed_time_;
    bool matched_;
};

/**
 * \class Market
 * \brief Represents a matching market
 *
 * \details TODO
 *
 * \remarks TODO
 */
class Market {
public:
    /**
     * \brief Default constructor
     *
     * \note will not be used; only exists for compilation
     */
    Market();

    /**
     * \brief Parameterized constructor
     *
     * \param m "Agents arrive at the market at rate m according to a Poisson process, i.e., in any interval [t, t + 1], m new agents enter the market in expectation"
     * 
     * \note
     */
    Market(uint64_t lambda, uint64_t m, uint64_t d, uint64_t delta);

    /**
     * \brief Adds an agent to the model
     * 
     * \param a pointer to an agent to add
     * 
     * \note
     */
    void add_agent(Agent* a);

    /**
     * \brief Removes an agent from the model
     * 
     * \param a pointer to an agent to remove
     * 
     * \note
     */
    void remove_agent(Agent* a);

    /**
     * \brief Models one time step in the model
     * 
     * \return increase in total utility
     * 
     * \note
     */
    float time_step();

private:
    uint64_t m_;
    uint64_t d_;
    uint64_t delta_;
    float utility_total_;
    std::poisson_distribution<int> lifespan_dist_;
    std::poisson_distribution<int> new_agent_dist_;
    std::default_random_engine generator_;
    // a graph representing compatable agents represented by an adjacency list
    std::list<std::list<Agent*>> compat_graph_;
};

#endif // MATCHING_HPP_INCLUDED