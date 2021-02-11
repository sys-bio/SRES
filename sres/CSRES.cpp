//
// Created by Ciaran on 05/02/2021.
//

#include "CSRES.h"

#include <cstring>
#include <iostream>

namespace opt {

    char *SRES_getLastError() {
        if (LAST_ERROR.empty()) {
            return nullptr;
        }
        char *cstr = (char *) malloc((LAST_ERROR.size() + 1) * sizeof(char *));
        strcpy(cstr, LAST_ERROR.c_str());
        return cstr;
    }

    SRES *SRES_newSRES(CostFunction cost, int populationSize, int numGenerations,
                       double *startingValues, const double *lb, double *ub, int numEstimatedParameters,
                       int childrate, int stopAfterStalledGenerations, bool logspace,
                       bool verbose) {
        try {
            std::vector<double> startVals(startingValues, startingValues + numEstimatedParameters);
            std::vector<double> lb_(lb, lb + numEstimatedParameters);
            std::vector<double> ub_(ub, ub + numEstimatedParameters);


            SRES *sres = new SRES(cost, populationSize, numGenerations, startVals, lb_, ub_, childrate,
                                  stopAfterStalledGenerations, logspace, verbose);
            return sres;
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            return reinterpret_cast<SRES *>(1);
        }
    }

    int SRES_deleteSRES(SRES *sres) {
        delete sres;
        sres = nullptr;
        return 0;

    }

    double SRES_getBestFitnessValue(SRES *sres) {
        try {
            return sres->getBestFitnessValue();
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
        }
    }

    bool SRES_fit(SRES *sres) {
        try {
            return sres->fit();
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            return -1;
        }
    }

    bool SRES_fitLHS(SRES *sres) {
        try {
            return sres->fitLHS();
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            return -1;
        }
    }

    int SRES_setSeed(SRES *sres, unsigned long long seed) {
        try {
            sres->setSeed(seed);
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            return -1;
        }
    }

    double *SRES_getHallOfFame(SRES *sres) {
        try {

            // hof is variable sized, so its better to malloc here
            auto hof = (double *) malloc(sizeof(double) * sres->getHallOfFame().size());
            const auto &v = sres->getHallOfFame();
            std::copy(v.begin(), v.end(), hof);
            return hof;
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
        }
    }

    int SRES_freeHallOfFame(double *hof) {
        if (hof)
            free(hof);
        hof = nullptr;
        return 0;
    }

    int SRES_getSizeOfHallOfFame(SRES *sres) {
        try {
            return sres->getHallOfFame().size();
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            return -1;
        }
    }

    int SRES_getSizeOfSolution(SRES *sres) {
        try {
            return sres->getSolutionValues().size();
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            return -1;
        }
    }


    double *SRES_getSolution(SRES *sres) {
        try {
            const std::vector<double> &sol = sres->getSolutionValues();
            auto solution = (double *) malloc(sizeof(double) * sol.size());
            std::copy(sol.begin(), sol.end(), solution);
            return solution;
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            // return??
        }
    }

    int SRES_freeSolution(double *sol) {
        free(sol);
        sol = nullptr;
        return 0;
    }

    int freeStuff(void *stuff) {
        if (stuff) {
            free(stuff);
            stuff = nullptr;
        }
        return 0;
    }

    int SRES_getNumberOfEstimatedParameters(SRES *sres) {
        try {
            return sres->getNumberOfParameters();
        } catch (std::exception &e) {
            LAST_ERROR = e.what();
            return -1;
        }
    }

}































