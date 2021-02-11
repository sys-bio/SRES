//
// Created by Ciaran on 06/02/2021.
//

#ifndef SRES_EVOLUTIONARYOPTIMIZER_H
#define SRES_EVOLUTIONARYOPTIMIZER_H

#include "Optimizer.h"

namespace opt {


    class EvolutionaryOptimizer : public Optimizer {

    public:

        /**
         * @brief default constructor for EvolutionaryOptimizer
         */
        EvolutionaryOptimizer() = default;

        /**
         * @brief default destructor for EvolutionaryOptimizer
         */
        ~EvolutionaryOptimizer() override = default;

        /**
         * @brief Construct an EvolutionaryOptimizer from
         * @param cost a function to minimize. @see CostFunction
         * @param populationSize The size of parent population.
         * @param numGenerations The number of generations to run
         * @param startingValues Parameter starting values.
         * @param lb lower boundary for each parameter during optimization
         * @param ub upper boundary for each parameter during optimization
         * @param childRate Number of times to multiply populationSize by to get the number of children per generation
         * defaults to 7.
         * @param stopAfterStalledGenerations stop evolution after this many generations if fitness does not change.
         */
        EvolutionaryOptimizer(
                CostFunction cost,
                int populationSize,
                int numGenerations,
                const DoubleVector &startingValues,
                const DoubleVector &lb,
                const DoubleVector &ub,
                int childRate = 7,
                int stopAfterStalledGenerations = 25,
                bool logspace = false
        );

        /**
         * @brief entry method for optimizer. Subclasses implement their
         * evolutionary strategy here. Users call the fit method to optimize
         * their parameters
         */
        bool fit() override = 0;

        /**
         * @brief getter for population size
         */
        [[nodiscard]] int getPopulationSize() const;

        /**
         * @brief setter for population size
         */
        void setPopulationSize(int populationSize);

        /**
         * @brief getter for child rate
         */
        [[nodiscard]] int getChildRate() const;

        /**
         * @brief setter for child rate
         */
        void setChildRate(int childrate);

        /**
         * @brief getter for number of generations
         */
        [[nodiscard]] int getNumGenerations() const;

        /**
         * @brief setter for number of generations
         */
        void setNumGenerations(int numGenerations);

        /**
         * @brief getter for current generation
         */
        [[nodiscard]] int getCurrentGeneration() const;

        /**
         * @brief setter for current generation
         */
        void setCurrentGeneration(int currentGeneration);

        /**
         * @brief getter for stopAfterStalledGenerations
         */
        [[nodiscard]] unsigned int getStopAfterStalledGenerations() const;

        /**
         * @brief setter for stopAfterStalledGenerations
         */
        void setStopAfterStalledGenerations(unsigned int stopAfterStalledGenerations);

        /**
         * @brief getter for individuals matrix
         */
        [[nodiscard]] const DoubleMatrix &getPopulation() const;

        /**
         * @brief setter for individuals matrix
         */
        void setPopulation(const DoubleMatrix &individuals);

        /**
         * @brief getter for population fitness
         */
        [[nodiscard]] const DoubleVector &getPopulationFitness() const;

        /**
         * @brief setter for population fitness
         */
        void setPopulationFitness(const DoubleVector &populationFitness);

    protected:

        /**
         * @brief Evaluate the fitness of @param individual
         * @details calls the cost function provided by the user.
         * All data and simulation procedures are handled by the
         * user inside the cost function. This method only
         * calls the cost function. A viable default is provided
         * but individual algorithms can implement their own
         * evaluate method.
         */
        virtual bool evaluate(std::vector<double> individual);

        /**
         * @brief Perform the mutation operation
         * implemented by a EvolutionaryOptimizer
         */
        virtual bool mutate() = 0;

        /**
         * @brief initialization routines that need to be
         * done before optimization can take place.
         * todo consider moving to constructor?
         */
        virtual bool initialize() = 0;

        /**
         * @brief find index of the best individual in the
         * current population.
         * @details the bestValue_ is updated by this method.
         */
        virtual size_t findBestIndividual() = 0;

        /**
         * @brief Perform the selection operation
         * implemented by a EvolutionaryOptimizer
         */
        virtual void select() = 0;

        /**
         * @brief counter for number of generations.
         */
        int currentGeneration_ = 0;

        /**
        * if no improvement was made after # stalled generations
        * stop
        */
        unsigned int stopAfterStalledGenerations_ = 25;

        /**
         * @brief the number of individuals in the parent population.
         * @details gets multiplied by childrate for
         * the child population size.
         */
        int populationSize_ = 100;

        /**
         * @brief the number of generations.
         * This is the stopping criteria for SRES.
         */
        int numGenerations_ = 500;

        /**
         * @brief matrix on individuals where rows are parameter
         * sets while columns are parameter candidates. In
         * other words, each row in this matrix is a candidate
         * solution.
         */
        DoubleMatrix population_;

        /**
         * @brief A vector containing all the fitness
         * for each row of the individuals_ matrix.
         */
        DoubleVector populationFitness_;

        /**
         * @brief the number of times to multiply
         * the populationSize by for the number of
         * individuals in the child population.
         */
        int childRate_ = 7;

    };

}

#endif //SRES_EVOLUTIONARYOPTIMIZER_H
