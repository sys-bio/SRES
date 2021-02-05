//
// Created by Ciaran on 05/02/2021.
//

#ifndef SRES_CSRES_H
#define SRES_CSRES_H

#include "SRES.h"

namespace csres {

    std::string LAST_ERROR;

#ifdef __cplusplus
    extern "C" {
#endif

    void setLastError(std::string err);

    char *getLastError();

    void clearLastError(const std::string &err);

    SRES *newSRES(SRES::CostFunction cost, int populationSize, int numGenerations,
                  double *startingValues, const double *lb, double *ub, int childrate = 7);

    void deleteSRES(SRES* sres);

    const double *getSolutionValues(SRES* sres);

    double* getBestValue(SRES* sres);

    bool fit(SRES* sres);

    void freeSolutionValues(const double *sol) ;

    void freeBestValue(const double *val);

    void setSeed(SRES* sres, unsigned long long seed);


    }

}


#endif //SRES_CSRES_H
