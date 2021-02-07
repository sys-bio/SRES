//
// Created by Ciaran on 05/02/2021.
//

#ifndef SRES_CSRES_H
#define SRES_CSRES_H

#include "SRES.h"

namespace opt {

    std::string LAST_ERROR;

#ifdef __cplusplus
    extern "C" {
#endif

    char *SRES_getLastError();

    SRES *SRES_newSRES(CostFunction cost, int populationSize, int numGenerations,
                       double *startingValues, const double *lb, double *ub,
                       int numEstimatedParameters, int childrate = 7);

    int SRES_setSeed(SRES *sres, unsigned long long seed);


    double *SRES_getSolution(SRES *sres);

    int SRES_getBestFitnessValue(SRES *sres, double *bestFitness);

    /**
     * The returned pointer is malloc'd and must be freed with
     * SRES_freeHallOfFame by the caller
     */
    double *SRES_getHallOfFame(SRES *sres);

    int SRES_getSizeOfHallOfFame(SRES *sres);

    int SRES_getSizeOfSolution(SRES *sres);

    bool SRES_fit(SRES *sres);

    int SRES_getNumberOfEstimatedParameters(SRES *sres);

    int freeStuff(void *stuff);

    int SRES_freeSolution(double *solution);

    int SRES_freeHallOfFame(double *hof);

    int SRES_deleteSRES(SRES *sres);
    }

}


#endif //SRES_CSRES_H
