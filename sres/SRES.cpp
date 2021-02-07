//
// Created by Ciaran on 05/02/2021.
//

#include "SRES.h"
#include <vector>
#include <iostream>

namespace opt {
    SRES::SRES(CostFunction cost, int populationSize,
               int numGenerations, const DoubleVector &startingValues, const DoubleVector &lb,
               const DoubleVector &ub, int childrate)
            : EvolutionaryOptimizer(cost, populationSize, numGenerations, startingValues, lb, ub, childrate) {};

    const DoubleVector &SRES::getMaxVariance() const {
        return maxVariance_;
    }

    void SRES::setMaxVariance(const DoubleVector &maxVariance) {
        maxVariance_ = maxVariance;
    }

    double SRES::getPf() const {
        return pf_;
    }

    void SRES::setPf(double pf) {
        pf_ = pf;
    }

    bool SRES::swap(size_t from, size_t to) {
        DoubleVector pTmp = population_[to];
        population_[to] = population_[from];
        population_[from] = pTmp;

        pTmp = variance_[to];
        variance_[to] = variance_[from];
        variance_[from] = pTmp;

        double dTmp = populationFitness_[to];
        populationFitness_[to] = populationFitness_[from];
        populationFitness_[from] = dTmp;

        dTmp = phi_[to];
        phi_[to] = phi_[from];
        phi_[from] = dTmp;

        return true;
    }

    bool SRES::replicate() {
        bool Continue = true;

        size_t Parent;
        size_t i, j;

        auto itSrc = population_.begin();
        auto endSrc = itSrc + populationSize_;
        auto itTarget = endSrc;
        auto itSrcVariance = variance_.begin();
        auto itTargetVariance = itSrcVariance + populationSize_;

        double *pVariance;
        double *pVarianceEnd;
        double *pParentVariance;

        // iterate over parents
        for (i = 0; itSrc != endSrc && Continue; ++itSrc, ++itSrcVariance, ++i) {
            // iterate over the child rate - 1 since the first child is the parent.
            for (j = 1; j < getChildRate(); ++j, ++itTarget, ++itTargetVariance) {
                // first just copy the individuals
                *itTarget = *itSrc;
                *itTargetVariance = *itSrcVariance;

                // do recombination on the sigma
                // since sigmas already have one parent's component
                // need only average with the sigmas of the other parent
                Parent = RandomNumberGenerator::getInstance().uniformInt(0, i + populationSize_ - 1);

                // extract the pointer to first element from itTargetVariance
                pVariance = itTargetVariance->data();
                pVarianceEnd = pVariance + numberOfParameters_;
                // extract pointer to Parent element of variance vector
                pParentVariance = variance_[Parent].data();

                for (; pVariance != pVarianceEnd; ++pVariance, ++pParentVariance)
                    *pVariance = (*pVariance + *pParentVariance) * .5;
            }
        }

        Continue &= mutate();

        return Continue;
    }

    bool SRES::mutate() {
        auto it = population_.begin() + populationSize_;
        auto end = population_.end();
        auto itVariance = variance_.begin() + populationSize_;

        double *pVariable, *pVariableEnd, *pVariance, *pMaxVariance;
//        double *pPhi = mPhi.array() + populationSize_;
        double *pPhi = &phi_[0] + populationSize_;
        double *pValue = &populationFitness_[0] + populationSize_;

        bool Continue = true;
        size_t i, j;
        double v1;

        // Mutate each new individual
        for (i = populationSize_; it != end && Continue; ++it, ++itVariance, ++i) {
            pVariable = it->data();
            pVariableEnd = pVariable + numberOfParameters_;
            pVariance = itVariance->data();
            pMaxVariance = maxVariance_.data();

            v1 = RandomNumberGenerator::getInstance().normal(0, 1);

            for (j = 0; pVariable != pVariableEnd; ++pVariable, ++pVariance, ++pMaxVariance, ++j) {
                double &mut = *pVariable;
                double Store = mut;


                const OptItem &OptItem = optItems_[j];

                size_t l = std::numeric_limits<size_t>::max();

                try {
                    // update the parameter for the variances
                    *pVariance = std::min(*pVariance * exp(
                            tauPrime_ * v1 + tau_ * RandomNumberGenerator::getInstance().normal(0, 1)
                    ), *pMaxVariance);

                    for (l = 0; l < 10; l++) {
                        // calculate the mutated parameter
                        mut = Store + *pVariance * RandomNumberGenerator::getInstance().normal(0, 1);

                        if (OptItem.checkConstraint(mut) == 0)
                            break;
                    }
                }

                catch (...) {
                    mut = (OptItem.getUb() + OptItem.getLb()) * 0.5;
                }

                if (l == 10)
                    mut = Store;

                /**
                 * todo The below was in the original code. Currently not sure if this applied to us.
                 */
                // We need to set the value here so that further checks take
                // account of the value.
                // *mProblemContext.master()->getContainerVariables()[j] = (mut);
            }

            // calculate its fitness
            Continue = evaluate(*it);
            *pValue++ = fitnessValue_;
            *pPhi++ = phi(i);
        }

        return Continue;
    }


    // evaluate the distance of parameters and constraints to boundaries
    double SRES::phi(size_t indivNum) {
        double phiVal = 0.0;
        double phiCalc;

        auto it = optItems_.begin();
        auto end = optItems_.end();
        double *pValue = population_[indivNum].data();

        for (; it != end; ++it, pValue++) {
            switch (it->checkConstraint()) {
                case -1:
                    phiCalc = it->getLb() - *pValue;
                    phiVal += phiCalc * phiCalc;
                    break;

                case 1:
                    phiCalc = *pValue - it->getLb();
                    phiVal += phiCalc * phiCalc;
                    break;
            }
        }

        /**
         * If / when we add constraints into the fray, we add them here.
         */
        // it = mProblemContext.master()->getConstraintList().begin();
        // end = mProblemContext.master()->getConstraintList().end();
        // for (; it != end; ++it) {
        //     phiCalc = (*it)->getConstraintViolation();
        //     if (phiCalc > 0.0)
        //         phiVal += phiCalc * phiCalc;
        // }

        return phiVal;
    }

    bool SRES::initialize() {
        size_t i;

        if (pf_ < 0.0 || 1.0 < pf_) {
            pf_ = 0.475;
        }

        population_.resize(childRate_ * populationSize_);

        for (i = 0; i < childRate_ * populationSize_; i++)
            population_[i] = std::vector<double>(numberOfParameters_);

        variance_.resize(childRate_ * populationSize_);

        for (i = 0; i < childRate_ * populationSize_; i++)
            variance_[i] = std::vector<double>(numberOfParameters_);

        maxVariance_.resize(numberOfParameters_);

        for (i = 0; i < numberOfParameters_; i++) {
            const OptItem &optItem = optItems_[i];
            try {
                maxVariance_[i] =
                        (optItem.getUb() - optItem.getLb()) / sqrt(double(numberOfParameters_));
            }
            catch (...) {
                maxVariance_[i] = 1.0e3;
            }
        }

        populationFitness_.resize(childRate_ * populationSize_);
        populationFitness_.assign(populationFitness_.size(), std::numeric_limits<double>::infinity());
        bestFitnessValue_ = std::numeric_limits<double>::infinity();
        hallOfFame_.push_back(bestFitnessValue_);

        phi_.resize(childRate_ * populationSize_);

        try {

            // double alpha = 0.2;
            // double chi = 1 / (2 * sqrt(double(numberOfParameters_))) + 1 / (2 * double(numberOfParameters_));
            // double varphi = sqrt(2/chi * log(1/alpha) *exp(chi/2 -(1-alpha)));

            double varphi = 1; // or just set varphi to 1
            tau_ = varphi / sqrt(2 * sqrt(double(numberOfParameters_)));
            tauPrime_ = varphi / sqrt(2 * double(numberOfParameters_));
        } catch (...) {
            tau_ = tauPrime_ = 1;
        }
        return true;
    }

    // Initialize the population
    bool SRES::creation(size_t first) {
        size_t i;
        size_t j;

        double mn;
        double mx;
        double la;

        bool Continue = true;

        auto it = population_.begin() + first;
        auto end = population_.begin() + populationSize_;
        auto itVariance = variance_.begin() + first;

        double *pVariable, *pVariableEnd, *pVariance, *pMaxVariance;
        double *pPhi = phi_.data() + first;
        double *pValue = populationFitness_.data() + first;

        // set the first individual to the initial guess
        if (it == population_.begin()) {
            pVariable = it->data();
            pVariableEnd = pVariable + numberOfParameters_;
            pVariance = itVariance->data();
            pMaxVariance = maxVariance_.data();

            bool pointInParameterDomain = true;

            for (j = 0; pVariable != pVariableEnd; ++pVariable, ++pVariance, ++pMaxVariance, ++j) {
                double &mut = *pVariable;
                const OptItem &optItem = optItems_[j];

                mut = optItem.getStartingValue();

                // force it to be within the bounds
                switch (optItem.checkConstraint(mut)) {
                    case -1:
                        mut = optItem.getLb();

                        if (!optItem.checkLowerBound(mut)) // Inequality
                        {
                            if (mut == 0.0)
                                mut = std::numeric_limits<double>::min();
                            else
                                mut += mut * std::numeric_limits<double>::epsilon();
                        }

                        pointInParameterDomain = false;

                        break;

                    case 1:
                        mut = optItem.getUb();

                        if (!optItem.checkUpperBound(mut)) // Inequality
                        {
                            if (mut == 0.0)
                                mut = -std::numeric_limits<double>::min();
                            else
                                mut -= mut * std::numeric_limits<double>::epsilon();
                        }

                        pointInParameterDomain = false;

                        break;
                }

                // (CW) not sure if we need this outside context of copasi
                // We need to set the value here so that further checks take
                // account of the value.
                // *mProblemContext.master()->getContainerVariables()[j] = (mut);

                // Set the variance for this parameter.
                *pVariance = std::min(optItem.getUb() - mut, mut - optItem.getLb()) /
                             sqrt(double(numberOfParameters_));
            }

            Continue = evaluate(*it);
            *pValue++ = fitnessValue_;
            *pPhi++ = phi(0);

            ++it;
            ++itVariance;
            ++first;
        }

        for (i = first; it < end; ++it, ++itVariance, ++i) {
            pVariable = it->data();
            pVariableEnd = pVariable + numberOfParameters_;
            pVariance = itVariance->data();
            pMaxVariance = maxVariance_.data();

            for (j = 0; pVariable != pVariableEnd; ++pVariable, ++pVariance, ++pMaxVariance, ++j) {
                double &mut = *pVariable;
                const OptItem &OptItem = optItems_[j];

                // calculate lower and upper bounds
                mn = OptItem.getLb();
                mx = OptItem.getUb();

                try {
                    // First determine the location of the intervall
                    // Secondly determine whether to distribute the parameter linearly or not
                    // depending on the location and act uppon it.
                    if (0.0 <= mn) // the interval [mn, mx) is in [0, inf)
                    {
                        la = log10(mx) - log10(std::max(mn, std::numeric_limits<double>::min()));

                        if (la < 1.8 || mn <= 0.0) // linear
                            mut = mn + RandomNumberGenerator::getInstance().uniformReal(0, 1) * (mx - mn);
                        else
                            mut = pow(10.0, log10(std::max(mn, std::numeric_limits<double>::min())) +
                                            la * RandomNumberGenerator::getInstance().uniformReal(0, 1));
                    } else if (mx > 0) // 0 is in the interval (mn, mx)
                    {
                        la = log10(mx) + log10(-mn);

                        if (la < 3.6) // linear
                            mut = mn + RandomNumberGenerator::getInstance().uniformReal(0, 1) * (mx - mn);
                        else {
                            double mean = (mx + mn) * 0.5;
                            double sigma = mean * 0.01;

                            do {
                                mut = RandomNumberGenerator::getInstance().normal(mean, sigma);
                            } while ((mut < mn) || (mut > mx));
                        }
                    } else // the interval (mn, mx] is in (-inf, 0]
                    {
                        // Switch lower and upper bound and change sign, i.e.,
                        // we can treat it similarly as location 1:
                        mx = -OptItem.getLb();
                        mn = -OptItem.getUb();

                        la = log10(mx) - log10(std::max(mn, std::numeric_limits<double>::min()));

                        if (la < 1.8 || mn <= 0.0) // linear
                            mut = -(mn + RandomNumberGenerator::getInstance().uniformReal(0, 1) * (mx - mn));
                        else
                            mut = -pow(10.0, log10(std::max(mn, std::numeric_limits<double>::min())) +
                                             la * RandomNumberGenerator::getInstance().uniformReal(0, 1));
                    }
                }

                catch (...) {
                    mut = (mx + mn) * 0.5;
                }

                // force it to be within the bounds
                switch (OptItem.checkConstraint(mut)) {
                    case -1:
                        mut = OptItem.getLb();

                        if (!OptItem.checkLowerBound(mut)) // Inequality
                        {
                            if (mut == 0.0)
                                mut = std::numeric_limits<double>::min();
                            else
                                mut += mut * std::numeric_limits<double>::epsilon();
                        }

                        break;

                    case 1:
                        mut = OptItem.getUb();

                        if (!OptItem.checkUpperBound(mut)) // Inequality
                        {
                            if (mut == 0.0)
                                mut = -std::numeric_limits<double>::min();
                            else
                                mut -= mut * std::numeric_limits<double>::epsilon();
                        }

                        break;
                }

                /*
                 * todo check if needed outside context of copasi
                 */
                // We need to set the value here so that further checks take
                // account of the value.
                // *mProblemContext.master()->getContainerVariables()[j] = (mut);

                // Set the variance for this parameter.
                *pVariance = std::min(OptItem.getUb() - mut, mut - OptItem.getLb()) /
                             sqrt(double(numberOfParameters_));
            }

            // calculate its fitness
            Continue = evaluate(*it);
            *pValue++ = fitnessValue_;
            *pPhi++ = phi(i);
        }

        return Continue;
    }

    // check the best individual at this generation
    size_t SRES::findBestIndividual() {
        size_t i, bestIndex = std::numeric_limits<size_t>::max();
        double bestValue = std::numeric_limits<double>::max();

        for (i = 0; i < populationSize_; i++)
            if (populationFitness_[i] < bestValue && phi_[i] == 0) {
                bestIndex = i;
                bestValue = populationFitness_[i];
                solutionValues_ = population_[i];
            }

        return bestIndex;
    }

    void SRES::select() {
        size_t i, j;
        size_t TotalPopulation = population_.size();
        bool wasSwapped;
        size_t sweepNum = TotalPopulation;  // This is default based on paper

        // Selection Method for Stochastic Ranking
        // stochastic ranking "bubble sort"

        for (i = 0; i < sweepNum; i++) // Here sweepNum is optimal number of sweeps from paper
        {
            wasSwapped = false;

            // :TODO: since we are only interested in mPopulationSize highest ranked
            // individuals the upper limit of the loop can be improved.
            for (j = 0; j < TotalPopulation - 1; j++)  // lambda is number of individuals
            {
                if ((phi_[j] == 0 && phi_[j + 1] == 0) || // within bounds
                    (RandomNumberGenerator::getInstance().uniformReal(0, 1) <
                     pf_))      // random chance to compare values outside bounds
                {
                    // compare obj fcn using mValue alternative code
                    if (populationFitness_[j] > populationFitness_[j + 1]) {
                        swap(j, j + 1);
                        wasSwapped = true;
                    }
                } else {
                    if (phi_[j] > phi_[j + 1]) // j further outside then j+1
                    {
                        swap(j, j + 1);
                        wasSwapped = true;
                    }
                }
            }

            // if nothing was swapped, then they're ordered!
            if (!wasSwapped) break;
        }
    }


    bool SRES::fit() {
        bool Continue = true;
        size_t bestIndex = std::numeric_limits<size_t>::max();

        size_t Stalled = 0;

        if (!initialize()) {
            // comment out mpCallBack code for now. Its to do with
            // logging the results and I may roll my own.
            // if (mpCallBack)
            //    mpCallBack->finishItem(mhGenerations);

            return false;
        }

        // initialise the population
        Continue = creation(0);

        // initialise solution variables. (cw not the same as original)
        bestFitnessValue_ = populationFitness_[0];
        solutionValues_ = population_[0];
        hallOfFame_.push_back(populationFitness_[0]);

        if (bestFitnessValue_ == -std::numeric_limits<double>::infinity())
            Continue = false;


        // get the index of the fittest
        bestIndex = findBestIndividual();

        if (bestIndex != std::numeric_limits<size_t>::max()) {
            // and store that value
            bestFitnessValue_ = populationFitness_[bestIndex];
            solutionValues_ = population_[bestIndex];
            hallOfFame_.push_back(populationFitness_[bestIndex]);

            if (bestFitnessValue_ == -std::numeric_limits<double>::infinity())
                Continue = false;


            // todo will need something to replace this
            // We found a new best value lets report it.
            // mpParentTask->output(COutputInterface::DURING);
        }

        if (!Continue) {
            // if (mpCallBack)
            //    mpCallBack->finishItem(mhGenerations);

            return true;
        }

        for (currentGeneration_ = 2;
             currentGeneration_ <= numGenerations_ && Continue;
             currentGeneration_++, Stalled++) {

            if (stopAfterStalledGenerations_ != 0 && Stalled > stopAfterStalledGenerations_)
                break;

            Continue = replicate();

            // select the most fit
            select();

            // get the index of the fittest
            bestIndex = findBestIndividual();

            if (bestIndex != std::numeric_limits<size_t>::max() && populationFitness_[bestIndex] < bestFitnessValue_) {
                if (bestIndex != std::numeric_limits<size_t>::max()) {
                    // and store that value
                    bestFitnessValue_ = populationFitness_[bestIndex];
                    hallOfFame_.push_back(populationFitness_[bestIndex]);
                    solutionValues_ = population_[bestIndex];
                    if (bestFitnessValue_ == -std::numeric_limits<double>::infinity())
                        Continue = false;
                }

                // if (mpCallBack)
                //    Continue = mpCallBack->progressItem(mhGenerations);

                //use a different output channel. It will later get a proper enum name
                // mpParentTask->output(COutputInterface::MONITORING);
            }

            //if (mLogVerbosity > 0)
            //    mMethodLog.enterLogEntry(
            //            COptLogEntry("Algorithm finished.",
            //                         "Terminated after " + std::to_string(currentGeneration_ - 1) + " of " +
            //                         std::to_string(mGenerations) + " generations."));

            //if (mpCallBack)
            //    mpCallBack->finishItem(mhGenerations);

        }

        return true;
    }


}
