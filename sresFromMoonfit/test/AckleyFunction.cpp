#include "ESES.h"
#include <cmath>
#include <math.h>
#include "random"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253
#define E   2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178


double AckleyFunction(double x, double y) {
    return -20 * exp( -0.2*( sqrt(  ( 0.5* (pow(x, 2) * pow(x, 2) ))) ) ) \
        - exp( 0.5 * ( cos(2*PI*x) + cos(2*PI*y) ) ) + E + 20;

};

// the global minumum is at x == y == 0
// we update this in the cost function
double best_cost = 1000000000.0;

void cost(double *input_params, double *output_fitness, double *constraints) {
    double cost = AckleyFunction(input_params[0], input_params[1]);
    *output_fitness = cost;
    if (cost < best_cost){
        best_cost = cost;
    }
}


double doNothingTransform(double x) {
    return x;
}

int main() {

    // known minimum:
    double zero_zero = AckleyFunction(0.0, 0.0);
    double zero_minus1 = AckleyFunction(0.0, -1.0);
    printf("f(0,0) = %.17g\n", zero_zero);

    // best min from SRES
    printf("f(0,-1) = %.17g\n", zero_minus1);
    printf("f(0,-1) = %.17g\n", AckleyFunction(0.0, -3.0));

    int parent_pop = 200; // miu
    int child_pop = 7 * parent_pop; // lambda
    int ngen = 100;
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

    double* op = ESGetBestParameters(stats);
    printf("best parameters: \t");
    for(int i=0; i<dim; i++){
        printf("%f\t", op[i]);
    }

    ESDeInitial(param, population, stats);
    freeTransformFun(trsfm);
//
//    freeData(best_eval);

}