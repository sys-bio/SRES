#include "ESES.h"
#include <cmath>
#include "random"

int N = 10;


/**
 * minimum = f(3, 0.5) = 0
 */
double BealeFunction(double x, double y) {
    double first = pow(1.5 - x + x*y, 2);
    double second = pow(2.25 - x + x*pow(y, 2), 2);
    double third = pow(2.625 - x + x*pow(y, 3), 2);
    return first + second + third;
};



void cost(double *input_params, double *output_fitness, double *constraints) {
    double cost = BealeFunction(input_params[0], input_params[1]);
    *output_fitness = cost;
}


double doNothingTransform(double x) {
    return x;
}

int main() {
    printf("f(3.0, 0.5) = %f\n", BealeFunction(3.0, 0.5));

    int parent_pop = 200; // miu
    int child_pop = 7 * parent_pop; // lambda
    int ngen = 25;
    unsigned int seed = 0; // == random
    int es = 0; // 0 or 1 to switch between two strategies involving lambda and mu.
    int constraint = 0;
    int dim = 2;
    double gamma = 0.85;
    double alpha = 0.2;
    int varphi = 1;
    int retry = 10;
    double pf = 0.475;
    // eslambda  = lambda + miu
    double ub[2] = {5, 5};
    double lb[2] = {-5, -5};
    ESParameter **param = makeESParameter();
    ESPopulation **population = makeESPopulation();
    ESStatistics **stats = makeESStatistics();
    ESfcnTrsfm *trsfm = makeTransformFun(dim);

    ESInitial(
            seed,
            param,
            trsfm,
            cost,
            es,
            constraint,
            dim,
            ub,
            lb,
            parent_pop,
            child_pop,
            ngen,
            gamma,
            alpha,
            varphi,
            retry,
            population,
            stats
    );
    unsigned int nbCostCalls = 0;

    while ((*stats)->curgen < (*param)->gen) {
        //(CW) multithreading opportunity
        ESStep(
                population,
                param,
                stats,
                pf,
                true
        );
        nbCostCalls++;
    }

    double *op = ESGetBestParameters(stats);
    printf("best parameters: \t");
    for (int i = 0; i < dim; i++) {
        printf("%f\t", op[i]);
    }

    ESDeInitial(param, population, stats);
    freeTransformFun(trsfm);
//
//    freeData(best_eval);

}