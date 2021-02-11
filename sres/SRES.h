//
// Created by Ciaran on 05/02/2021.
//

#ifndef SRES_SRES_H
#define SRES_SRES_H


#include <vector>
#include <random>
#include <functional>
#include "OptItems.h"
#include "EvolutionaryOptimizer.h"

namespace opt {

    class SRES : public EvolutionaryOptimizer {

    public:

        SRES() = default;

        SRES(CostFunction cost, int populationSize, int numGenerations,
             const DoubleVector &startingValues, const DoubleVector &lb,
             const DoubleVector &ub, int childrate,
             int stopAfterStalledGenerations = 0, bool logspace = false,
             bool verbose = false);

        [[nodiscard]] const DoubleVector &getMaxVariance() const;

        void setMaxVariance(const DoubleVector &maxVariance);

        [[nodiscard]] double getPf() const;

        void setPf(double pf);

        bool fit() override;

        bool fitLHS();

    private:
        bool swap(size_t from, size_t to);

        bool replicate();

        bool mutate() override;

        double phi(size_t indivNum);

        bool initialize() override;

        bool initializeLHS();

        bool creation(size_t first);

        size_t findBestIndividual() override;

        void select() override;

        /**
         * @brief variance of every position in the
         * populationi matrix
         * @note (probably correct, might not be)
         */
        DoubleMatrix variance_;

        /**
         * @brief largest variance for a particular population.
         * @note (maybe, is this the row/column oc variance_? todo look into what this is)
         */
        DoubleVector maxVariance_;

        /**
         * @brief parameter used in stochastic sorting.
         * @details usually has a value of 0.475
         */
        double pf_ = 0.475;

        double tau_ = 100.0;    // parameter for updating variances

        double tauPrime_ = 100.0;    // parameter for updating variances

        DoubleVector phi_;

        bool logspace_ = false;

        /**
         * Prints output during optimization when true
         */
        bool verbose_ = false;

        void printCurrent();
    };

}
#endif //SRES_SRES_H
