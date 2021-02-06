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

    void SRES_setLastError(std::string err);

    char *SRES_getLastError();

    void SRES_clearLastError(const std::string &err);

    SRES *SRES_newSRES(SRES::CostFunction cost, int populationSize, int numGenerations,
                  double *startingValues, const double *lb, double *ub,
                  int numEstimatedParameters, int childrate = 7);

    void SRES_deleteSRES(SRES* sres);

    double *SRES_getSolutionValues(SRES* sres);

    double* SRES_getBestValue(SRES* sres);

    bool SRES_fit(SRES* sres);

    void SRES_freeSolutionValues(const double *sol) ;

    void SRES_freeBestValue(const double *val);

    void SRES_setSeed(SRES* sres, unsigned long long seed);

    double *SRES_getTrace(SRES *sres, int sizeOfTrace);

    }

}


#endif //SRES_CSRES_H
