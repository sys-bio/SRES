//
// Created by Ciaran on 06/02/2021.
//

#include "EvolutionaryOptimizer.h"


namespace opt {


    EvolutionaryOptimizer::EvolutionaryOptimizer(
            CostFunction cost, int populationSize, int numGenerations,
            const DoubleVector &startingValues, const DoubleVector &lb,
            const DoubleVector &ub, int childRate, int stopAfterStalledGenerations)
            : Optimizer(cost, startingValues, lb, ub),
              populationSize_(populationSize),
              numGenerations_(numGenerations),
              childRate_(childRate),
              stopAfterStalledGenerations_(stopAfterStalledGenerations) {}


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


}