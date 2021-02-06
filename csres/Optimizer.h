//
// Created by Ciaran on 06/02/2021.
//

#ifndef SRES_OPTIMIZER_H
#define SRES_OPTIMIZER_H

#include "OptItems.h"
#include "RandomNumberGenerator.h"

/*
 * Its useful to be able to provide our own objective functions
 * but it introduces overhead. It would be nice to be able
 * to pass back an instance of roadrunner though.
 *
 * Could be naughty and upload the static rr dist
 * to github for linking?
 */

namespace opt {

    using DoubleMatrix = std::vector<std::vector<double>>;
    using DoubleVector = std::vector<double>;

    /**
     * @brief cost function with signature for parameters (to estimate).
     * @param individual or genome. This is a double vector representing candiate parameters
     * @return Fitness of the input individual.
     */
    typedef double(*CostFunction)(double *);


    /**
     * @brief base class for optimization algorithms.
     *
     */
    class Optimizer {

    public:

        /**
         * @brief default construct optimizer
         */
        Optimizer() = default;

        /**
         * @brief virutal destructor
         */
        virtual ~Optimizer() = default;

        /**
         * @brief construct an Optimizer from components required to make
         * up the OptItems. The OptItems are build from @param startingValues
         * @param lb and @param ub on an index by index basis. Therefore,
         * they must all be of the same size.
         */
        Optimizer(
                CostFunction cost, const DoubleVector &startingValues,
                const DoubleVector &lb, const DoubleVector &ub
        );

        /**
         * @brief entrance method for running
         * of the algorithm. Subclasses must provide
         * their own implementation.
         * @details class level variables are modified
         * during optimization.
         * @returns true on success.
         */
        virtual bool fit() = 0;

        /**
         * @brief construct an optimizer using the OptItems
         * container
         */
        Optimizer(CostFunction cost, OptItems optItems);

        /**
         * @brief getter for OptItems instance
         */
        [[nodiscard]] const OptItems &getOptItems() const;

        /**
         * @brief setter for OptItems instance
         */
        void setOptItems(const OptItems &optItems);

        /**
         * @brief getter for cost function
         */
        [[nodiscard]] CostFunction getCost() const;

        /**
         * @brief setter for cost function
         */
        void setCost(CostFunction cost);

        void setSeed(unsigned long long int seed);

        bool setSolution(const double &value, const std::vector<double> &variables);

        std::vector<double> getHallOfFame();

        [[nodiscard]] const std::vector<double> &getSolutionValues() const;

        void setSolutionValues(const std::vector<double> &solutionValues);

        [[nodiscard]] double getBestFitnessValue() const;

        void setBestValue(double bestValue);

        [[nodiscard]] int getNumberOfParameters() const;

        void setNumberOfParameters(int numberOfParameters);

    protected:

        /**
         * Collect the best values over generations
         */
        DoubleVector hallOfFame_;

        /**
         * @brief the parameters for the best solution
         */
        std::vector<double> solutionValues_;

        /**
        * @brief The fitness of the last evaluation.
        */
        double fitnessValue_ = 10000000;

        /**
         * @brief the fitness of the best evaluation so far
         */
        double bestFitnessValue_ = 10000000;

        /**
         * @brief optimization items. Defines the fitting start
         * values as well as lower and upper bounds. Also
         * has some methods to performs checks
         */
        OptItems optItems_;

        /**
         * @brief cost function. The input (double*) are parameter
         * values to try and the output are the fitness values of
         * those parameters.
         */
        CostFunction cost_; // way to inject cost function from Python

        /**
         * @brief how many parameters are in the estimation problem.
         * @details this information is available from the size of the
         * OptItems container which should be used instead.
         */
        int numberOfParameters_; // number of parameters to estimate



    };

}

#endif //SRES_OPTIMIZER_H
