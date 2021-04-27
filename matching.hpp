/**
 * -  For t >= 0, let At be the set of the agents in our market at time t, and let Zt := |At|. We refer to At as the pool of the market
 * - assume A_0 = \varnothing
 * - runs in interval [0,T]
 * - Agents arrive at the market at rate m according to a Poisson process, i.e., in any interval [t, t + 1], m new agents enter the market in expectation. Throughout the paper we assume m >= 1

*/

#ifndef MATCHING_HPP_INCLUDED
#define MATCHING_HPP_INCLUDED 1

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>

/**
 * \class Agent
 * \brief Represents an agent in our market
 *
 * \details TODO
 *
 * \remarks
 *   TODO
 */
class agent {
public:
    /**
     * \brief Default constructor
     *
     * \note will not be used; only exists for compilation
     */
    agent();
    /**
     * \brief Parameterized constructor
     *
     * \note
     */
    agent(uint64_t lifespan, float delta);

    /**
     * \brief Detructor
     *
     * \note
     */
    ~agent();

    /**
     * \brief utility function
     * 
     * \note = e^(delta*(elapsed_time))
     */
    utility();

private:
    uint64_t lifespan_;
    float delta_;
    uint64_t elapsed_time_;
}

/**
 * \class Market
 * \brief Represents a matching market
 *
 * \details TODO
 *
 * \remarks
 *   TODO
 */
class market {
public:
    /**
     * \brief Default constructor
     *
     * \note will not be used; only exists for compilation
     */
    market();

    /**
     * \brief Destructor
     * 
     * \note
     */
    ~market();

    /**
     * \brief Parameterized constructor
     *
     * \param m "Agents arrive at the market at rate m according to a Poisson process, i.e., in any interval [t, t + 1], m new agents enter the market in expectation"
     * 
     * \note
     */
    market(uint64_t lambda, uint64_t m, uint64_t d);

    /**
     * \brief Adds an agent to the model
     * 
     * \param a pointer to an agent to add
     * 
     * \note
     */
    void add_agent(agent* a);

    /**
     * \brief Removes an agent from the model
     * 
     * \param a pointer to an agent to remove
     * 
     * \note
     */
    void remove_agent(agent* a);

    /**
     * \brief Models one time step in the model
     * 
     * \return increase in total utility
     * 
     * \note
     */
    float time_step();

private:
    uint64_t lambda_; // parameter in random function for agent lifespans
    uint64_t m_;
    uint64_t d_;
    float utility_total_;
    // a graph representing compatable agents represented by an adjacency list
    std::list<std::list<agent*>> compat_graph_;
}

#endif // MATCHING_HPP_INCLUDED