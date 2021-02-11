//
// Created by Ciaran on 06/02/2021.
//

#include <iostream>
#include "EvolutionaryOptimizer.h"
#include <set>
#include <execution>

namespace opt {

    EvolutionaryOptimizer::EvolutionaryOptimizer(
            CostFunction cost, int populationSize, int numGenerations,
            const DoubleVector &startingValues, const DoubleVector &lb,
            const DoubleVector &ub, int childRate, int stopAfterStalledGenerations,
            bool logspace, bool verbose, int numLHSInitSamples,
            int numGenerationsForLHSInitSamples)
            : Optimizer(cost, startingValues, lb, ub, logspace, verbose),
              populationSize_(populationSize),
              numGenerations_(numGenerations),
              childRate_(childRate),
              stopAfterStalledGenerations_(stopAfterStalledGenerations),
              numLHSInitSamples_(numLHSInitSamples),
              numGenerationsForLHSInitSamples_(numGenerationsForLHSInitSamples) {
        if (stopAfterStalledGenerations_ == 1) {
            stopAfterStalledGenerations_ = ceil(numGenerations_ * 0.2); // defaults to 20% of generations
        } else if (stopAfterStalledGenerations == 0) {
            // set arbitrarily high so we'll never hit it
            stopAfterStalledGenerations_ = 1000000000;
        }
    }

    int EvolutionaryOptimizer::getPopulationSize() const {
        return populationSize_;
    }

    void EvolutionaryOptimizer::setPopulationSize(int populationSize) {
        EvolutionaryOptimizer::populationSize_ = populationSize;
    }

    int EvolutionaryOptimizer::getChildRate() const {
        return childRate_;
    }

    void EvolutionaryOptimizer::setChildRate(int childrate) {
        childRate_ = childrate;
    }

    int EvolutionaryOptimizer::getNumGenerations() const {
        return numGenerations_;
    }

    void EvolutionaryOptimizer::setNumGenerations(int numGenerations) {
        EvolutionaryOptimizer::numGenerations_ = numGenerations;
    }

    bool EvolutionaryOptimizer::evaluate(std::vector<double> individual) {
        bool Continue = true;

        // todo refactor this so that it returns the fitness instead

        /**
         * sadly, to make this interoperable with C and therefore
         * Python, I could not used std::function for the callback.
         * Instead we use a raw double, which means we need
         * to construct a raw double array from individual
         */
        double *pd = individual.data();

        fitnessValue_ = (*cost_)(pd);

        return Continue;
    }


    int EvolutionaryOptimizer::getCurrentGeneration() const {
        return currentGeneration_;
    }

    void EvolutionaryOptimizer::setCurrentGeneration(int currentGeneration) {
        currentGeneration_ = currentGeneration;
    }

    unsigned int EvolutionaryOptimizer::getStopAfterStalledGenerations() const {
        return stopAfterStalledGenerations_;
    }

    void EvolutionaryOptimizer::setStopAfterStalledGenerations(unsigned int stopAfterStalledGenerations) {
        stopAfterStalledGenerations_ = stopAfterStalledGenerations;
    }

    const DoubleMatrix &EvolutionaryOptimizer::getPopulation() const {
        return population_;
    }

    void EvolutionaryOptimizer::setPopulation(const DoubleMatrix &individuals) {
        population_ = individuals;
    }

    const DoubleVector &EvolutionaryOptimizer::getPopulationFitness() const {
        return populationFitness_;
    }

    void EvolutionaryOptimizer::setPopulationFitness(const DoubleVector &populationFitness) {
        EvolutionaryOptimizer::populationFitness_ = populationFitness;
    }

    DoubleMatrix EvolutionaryOptimizer::findStartingSet() {
        RandomNumberGenerator rng = RandomNumberGenerator::getInstance();


        DoubleMatrix bestPop;
        double bestTrial = 100000000; // arbitrarily large to start with

        for (int trial = 0; trial < numLHSInitSamples_; trial++) {

            // initialize vector to store fitnesses
//            std::vector<double> fitnesses(populationSize_ * childRate_);

            // sample using lhs
//            DoubleMatrix trialPop = rng.lhs(
//                    populationSize_ * childRate_,
//                    numberOfParameters_,
//                    optItems_.getLb(),
//                    optItems_.getUb()
//            );

            // evolve a little
//            fit();


//            // evaluate the fitnesses using C++17's cool parallel for_each =]
//            double fitnessOfThisTrail = 0;
//            std::for_each(std::execution::par, trialPop.begin(), trialPop.end(),
//                          [this, &fitnessOfThisTrail](auto &ind) {
//                              double *pd = ind.data();
//                              fitnessOfThisTrail += (*cost_)(pd);
//                          });
//
//            if (fitnessOfThisTrail < bestTrial) {
//                // we've found a better starting set
//                bestTrial = fitnessOfThisTrail;
//                bestPop = trialPop;
//            }
        }
        return bestPop;
    }


}