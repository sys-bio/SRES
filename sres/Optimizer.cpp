//
// Created by Ciaran on 06/02/2021.
//

#include <utility>
#include <vector>
#include <iostream>
#include "Optimizer.h"

namespace opt {

    Optimizer::Optimizer(
            opt::CostFunction cost, const std::vector<double> &startingValues,
            const std::vector<double> &lb, const std::vector<double> &ub, bool logspace,
            bool verbose)
            : cost_(cost),
              optItems_(OptItems(startingValues, lb, ub, logspace)),
              numberOfParameters_(startingValues.size()),
              verbose_(verbose){

    }

    Optimizer::Optimizer(opt::CostFunction cost, OptItems optItems)
            :
            cost_(cost),
            optItems_(std::move(optItems)),
            numberOfParameters_(optItems.size()) {}

    CostFunction Optimizer::getCost() const {
        return cost_;
    }

    void Optimizer::setCost(CostFunction cost) {
        cost_ = cost;
    }

    const OptItems &Optimizer::getOptItems() const {
        return optItems_;
    }

    void Optimizer::setOptItems(const OptItems &optItems) {
        optItems_ = optItems;
    }

    void Optimizer::setSeed(unsigned long long int seed) {
        RandomNumberGenerator::getInstance().setSeed(seed);
    }

    bool Optimizer::setSolution(const double &value,
                                const std::vector<double> &variables) {
        bestFitnessValue_ = value;
        hallOfFame_.push_back(bestFitnessValue_);

        // The initialization call from Optimizer and GASR have NULL as variables
        if (!variables.empty())
            solutionValues_ = variables;

        bool Continue = true;

        if (value == -std::numeric_limits<double>::infinity())
            Continue = false;

        // if (mpCallBack)
        //    Continue &= mpCallBack->progressItem(mhSolutionValue);

        return Continue;
    }


    const std::vector<double> &Optimizer::getSolutionValues() const {
        return solutionValues_;
    }

    void Optimizer::setSolutionValues(const std::vector<double> &solutionValues) {
        solutionValues_ = solutionValues;
    }

    double Optimizer::getBestFitnessValue() const {
        return bestFitnessValue_;
    }

    void Optimizer::setBestValue(double bestValue) {
        bestFitnessValue_ = bestValue;
    }

    std::vector<double> Optimizer::getHallOfFame() {
        return hallOfFame_;
    }

    int Optimizer::getNumberOfParameters() const {
        return numberOfParameters_;
    }

    void Optimizer::setNumberOfParameters(int numberOfParameters) {
        numberOfParameters_ = numberOfParameters;
    }

}


