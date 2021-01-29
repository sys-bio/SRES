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


    unsigned int seed = 1;
    int es = 1;
    int constraint = 0;
    int dim = 2;
    double gamma = 0.85;
    double alpha = 0.2;
    int varphi = 1;
    int retry = 10;
    double pf = 0.45;
    int parent_pop = 30; // miu
    int child_pop = 30; // lambda
    int ngen = 50;
    double ub[2] = {10.0, 10.0};
    double lb[2] = {0.1, 0.1};
    ESParameter **param = makeESParameter();
    ESPopulation **population = makeESPopulation();
    ESStatistics **stats = makeESStatistics();
    ESfcnTrsfm *trsfm = makeTransformFun(dim);

    ESInitial(
            seed,
            param,
            trsfm,
            simple_cost,
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
                pf
        );
        nbCostCalls++;
    }

    ESDeInitial(param, population, stats);
    freeTransformFun(trsfm, 2);

    freeData(EXP_DATA);



}