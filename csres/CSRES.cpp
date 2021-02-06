//
// Created by Ciaran on 05/02/2021.
//

#include "CSRES.h"

#include <cstring>
#include <iostream>

namespace csres {


    void SRES_setLastError(std::string err) {
        LAST_ERROR = std::move(err);
    }

    char *SRES_getLastError() {
        if (LAST_ERROR.empty()) {
            return nullptr;
        }
        char *cstr = (char *) malloc((LAST_ERROR.size() + 1) * sizeof(char *));
        strcpy(cstr, LAST_ERROR.c_str());
        return cstr;
    }

    void SRES_clearLastError(const std::string &err) {
        LAST_ERROR = "";
    }

    SRES *SRES_newSRES(SRES::CostFunction cost, int populationSize, int numGenerations,
                  double *startingValues, const double *lb, double *ub, int numEstimatedParameters, int childrate) {

        std::vector<double> startVals(startingValues, startingValues + numEstimatedParameters);
        std::vector<double> lb_(lb, lb + numEstimatedParameters);
        std::vector<double> ub_(ub, ub + numEstimatedParameters);

        SRES *sres = new SRES(cost, populationSize, numGenerations, startVals, lb_, ub_, childrate);
        return sres;
    }

    void SRES_deleteSRES(SRES *sres) {
        delete sres;
    }


    double *SRES_getSolutionValues(SRES *sres) {
        const std::vector<double> &sol = sres->getSolutionValues();
        auto *dsol = (double *) malloc(sizeof(double) * sol.size());
        for (int i = 0; i < sol.size(); i++) {
            dsol[i] = sol[i];
        }
        return dsol;
    }

    double *SRES_getBestValue(SRES *sres) {
        auto *bval = (double *) malloc(sizeof(double));
        double v = sres->getBestValue();
        *bval = v;
        return bval;
    }

    bool SRES_fit(SRES *sres) {
        return sres->fit();
    }

    void SRES_freeSolutionValues(const double *sol) {
        free((void *) sol);
    }

    void SRES_freeBestValue(const double *val) {
        free((void *) val);
    }

    void SRES_setSeed(SRES *sres, unsigned long long seed) {
        sres->setSeed(seed);
    }

    double *SRES_getTrace(SRES *sres, int sizeOfTrace) {
        auto *bval = (double *) malloc(sizeof(double) * sizeOfTrace);
        std::vector<double> v = sres->getTrace();
        for (int i=0; i<sizeOfTrace; i++){
            std::cout << "from C: v[i]: " << v[i]<<std::endl;
            *bval = v[i];
        }
        return bval;
    }

    // getVariance
}































