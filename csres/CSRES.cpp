//
// Created by Ciaran on 05/02/2021.
//

#include "CSRES.h"

#include <cstring>

namespace csres {


    void setLastError(std::string err) {
        LAST_ERROR = std::move(err);
    }

    char *getLastError() {
        if (LAST_ERROR.empty()) {
            return nullptr;
        }
        char *cstr = (char *) malloc((LAST_ERROR.size() + 1) * sizeof(char *));
        strcpy(cstr, LAST_ERROR.c_str());
        return cstr;
    }

    void clearLastError(const std::string &err) {
        LAST_ERROR = "";
    }

    SRES *newSRES(SRES::CostFunction cost, int populationSize, int numGenerations,
                  double *startingValues, const double *lb, double *ub, int childrate) {

        int sizeOfStartingValues = sizeof(startingValues) / sizeof(double);
        int sizeOfLb = sizeof(lb) / sizeof(double);
        int sizeOfUb = sizeof(ub) / sizeof(double);

        if ((sizeOfStartingValues != sizeOfLb) | (sizeOfStartingValues != sizeOfUb)) {
            std::string err = "Size of input arrays are not equal";
            setLastError(err);
        }

        std::vector<double> startVals(startingValues, startingValues + sizeOfUb);
        std::vector<double> lb_(lb, lb + sizeOfUb);
        std::vector<double> ub_(ub, ub + sizeOfUb);

        SRES *sres = new SRES(cost, populationSize, numGenerations, startVals, lb_, ub_);
        return sres;
    }

    void deleteSRES(SRES *sres) {
        delete sres;
    }


    const double *getSolutionValues(SRES *sres) {
        const std::vector<double> &sol = sres->getSolutionValues();
        auto *dsol = (double *) malloc(sizeof(double) * sol.size());
        for (int i = 0; i < sol.size(); i++) {
            dsol[i] = sol[i];
        }
        return dsol;
    }

    double *getBestValue(SRES *sres) {
        auto *bval = (double *) malloc(sizeof(double));
        double v = sres->getBestValue();
        *bval = v;
        return bval;
    }

    bool fit(SRES *sres) {
        return sres->fit();
    }

    void freeSolutionValues(const double *sol) {
        free((void *) sol);
    }

    void freeBestValue(const double *val) {
        free((void *) val);
    }

    void setSeed(SRES *sres, unsigned long long seed) {
        sres->setSeed(seed);
    }
}































