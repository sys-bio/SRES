//
// Created by Ciaran on 05/02/2021.
//

#ifndef SRES_SRES_H
#define SRES_SRES_H


#include <vector>
#include <random>
#include <functional>
#include "OptItems.h"

namespace csres {

    using DoubleMatrix = std::vector<std::vector<double>>;
    using DoubleVector = std::vector<double>;

    class SRES {

    public:
        /**
         * @brief cost function with signature for parameters (to estimate).
         * @param individual or genome. This is a double vector representing candiate parameters
         * @return Fitness of the input individual.
         */
        typedef double(*CostFunction)(double *);

        SRES() = default;

        SRES(CostFunction cost, int populationSize, int numGenerations,
             const DoubleVector &startingValues, const DoubleVector &lb,
             const DoubleVector &ub, int childrate = 7);

        bool swap(size_t from, size_t to);

        bool replicate();

        [[nodiscard]] int getPopulationSize() const;

        void setPopulationSize(int populationSize);

        [[nodiscard]] int getChildrate() const;

        void setChildrate(int childrate);


        [[nodiscard]] int getNumGenerations() const;

        void setNumGenerations(int numGenerations);

        [[nodiscard]] const std::vector<double> &getSolutionValues() const;

        void setSolutionValues(const std::vector<double> &solutionValues);

        [[nodiscard]] double getBestValue() const;

        void setBestValue(double bestValue);

        bool mutate();

        [[nodiscard]] const DoubleVector &getMaxVariance() const;

        void setMaxVariance(const DoubleVector &maxVariance);

        bool evaluate(std::vector<double> individual);

        double phi(size_t indivNum);

        bool fit();

        [[nodiscard]] double getPf() const;

        void setPf(double pf);

        bool initialize();

        bool creation(size_t first);

        size_t fittest();

        void setSeed(unsigned long long int seed);

    private:

        DoubleMatrix individuals_;
        DoubleMatrix variance_;
        DoubleVector maxVariance_;

        /**
         * @brief storage of the fitness value
         */
        DoubleVector values_;
        DoubleVector phi_;
        int populationSize_ = 100;
        int numGenerations_ = 500;

        int childrate_ = 7;
        double pf_;
        int numberOfParameters_; // number of parameters to estimate
        OptItems optItems_;


        /**
        * if no improvement was made after # stalled generations
        * stop
        */
        unsigned int stopAfterStalledGenerations_;

        /**
        * The value of the last evaluation.
        */
        double fitnessValue_;

        double bestValue_;

        std::vector<double> solutionValues_;

        double tau_;    // parameter for updating variances

        double tauPrime_;    // parameter for updating variances

        CostFunction cost_; // way to inject cost function from Python

        int currentGeneration_ = 0;

        void select();
    };

}
#endif //SRES_SRES_H
