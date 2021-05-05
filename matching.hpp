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
     * \param lambda the expected lifespan of each agent
     * \param m the (expected) rate at which new agents enter the market
     * \param d probability of compatability for agents
     * \param delta the loss constant for all agents in the market
     * \param greedy true for a market with a greedy strategy, false for one with a patient strategy
     * 
     * \note
     */
    Market(uint64_t lambda, uint64_t m, uint64_t d, uint64_t delta, bool greedy);

    /**
     * \brief Changes the greedy_ value for the market
     * 
     * \param strategy specifies the value of greedy_ going forward. The
     * strategy will be greedy if the value is "greedy", patient if the value
     * is "patient", and whatever the opposite of the current value is
     * otherwise.
     * 
     * \note
     */
    void change_strategy(std::string strategy);

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
     * \brief matches the agent if possible
     * 
     * \param a the agent to be matched
     * 
     * \return true if the agent is successfully matched, false otherwise
     * 
     */
    bool try_match(Agent* a);

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
    bool greedy_;
    float utility_total_;
    std::poisson_distribution<int> lifespan_dist_;
    std::poisson_distribution<int> new_agent_dist_;
    std::default_random_engine generator_;
    // a graph representing compatable agents represented by an adjacency list
    std::list<std::list<Agent*>> compat_graph_;
};

#endif // MATCHING_HPP_INCLUDED