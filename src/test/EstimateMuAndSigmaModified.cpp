#include "ESES.h"
#include <cmath>
#include "random"

#define PI 3.14159265359
#define Popsize 5
#define Generation 500
#define Gamma 0.85
#define Alpha 0.2
#define Varphi 1
#define Retry 10
#define ESPlus 0
#define ESSlash 1
#define Pf 0.45


double normal(double x, double mu, double sigma) {
    double first_bit = 1 / (sigma * sqrt(2 * PI));
    double second_bit = 0.5 * pow(((x - mu) / sigma), 2);
    return pow(first_bit, -second_bit);
};

double *generateData(double mu, double sigma) {
    auto *arr = (double *) malloc(sizeof(double) * 10);
    for (int i = 0; i < 10; i++) {
        arr[i] = normal(i, mu, sigma);
    }
    return arr;
}

void freeData(double *data) {
    free(data);
}

double *EXP_DATA = generateData(10, 0.1);

void simple_cost(double *input_params, double *output_fitness, double *constraints) {
    double *data = generateData(input_params[0], input_params[1]);
    double cost = 0.0;
    for (int i = 0; i < 10; i++) {
        cost += pow(EXP_DATA[i] - data[i], 2);
    }
    freeData(data);
    *output_fitness = cost;
}


double doNothingTransform(double x) {
    return x;
}

double log2Transform(double x) {
    return log2(x);
}

double log10Transform(double x) {
    return log10(x);
}


void func_to_pass_in(double *x, double* y) {
    printf("hello from func_to_pass_in\n");
    printf("x is %f\n", *x);
    printf("y is %f\n", *y);
    *y = 2* (*x);
}

int main() {

//    double input = 4.0;
//    double output = 0.0;
//    function_that_takes_a_function(func_to_pass_in, &input, &output);
//    printf("Output is still : %f\n", output);

    int i;
    ESParameter **param = makeESParameter();
    ESPopulation **population = makeESPopulation();
    ESStatistics **stats = makeESStatistics();
    ESfcnTrsfm *trsfm;
    unsigned int seed;
    int es;
    int constraint, dim;
    int miu, lambda;
    double gamma, alpha, varphi;
    int retry;
    double pf;

    seed = 0;
    gamma = Gamma;
    alpha = Alpha;
    varphi = Varphi;
    retry = Retry;
    pf = 0.45;
    es = ESPlus;
    int ngen = 50;

    constraint = 0;
    dim = 2;
    miu = 30;
    lambda = 20;
    double ub[2] = {10.0, 10.0};
    double lb[2] = {0.1, 0.1};


//    trsfm = (ESfcnTrsfm *) malloc(dim * sizeof(ESfcnTrsfm));
//    for (i = 0; i < dim; i++) {
//        trsfm[i] = doNothingTransform;
//    }
    trsfm = makeTransformFun(2);

    auto *simple_cost_ptr = (ESfcnFG *) malloc(sizeof(ESfcnFG));
    *simple_cost_ptr = simple_cost;

    ESInitial(
            seed,
            param,
            trsfm,
            *simple_cost_ptr,
            es,
            constraint,
            dim,
            ub,
            lb,
            miu,
            lambda,
            ngen,
            gamma,
            alpha,
            varphi,
            retry,
            population,
            stats
    );
    unsigned int nbCostCalls = 0;

//    while (derefESStatistics(stats)->curgen < derefESParameter(param)->gen) {
//        //(CW) multithreading opportunity
//        ESStep(
//                population,
//                param,
//                stats,
//                pf
//        );
//        nbCostCalls++;
//    }

//    ESDeInitial(derefESParameter(param), derefESPopulation(population), derefESStatistics(stats));

//    freeData(EXP_DATA);
    freeESParameter(param);
//    freeESStatistics(stats);
//    freePopulation(population);



}