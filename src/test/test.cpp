//
// Created by Ciaran on 29/01/2021.
//

#include "gtest/gtest.h"
#include "ESES.h"
#include <cmath>
//#include "random"

#define PI 3.14159265359

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

class SresTests : public ::testing::Test{

public:
    SresTests() = default;
};

TEST_F(SresTests, CheckThatESParameterCanBeCreatedAndDestroyedWithoutLeaks){
    // run this with valgrind / sanitizers
    ESParameter **  param = makeESParameter();
    freeESParameter(param);
}


TEST_F(SresTests, CheckThatEStatisticsCanBeCreatedAndDestroyedWithoutLeaks){
    // run this with valgrind / sanitizers
    ESStatistics **  stat = makeESStatistics();
    freeESStatistics(stat);
}


TEST_F(SresTests, CheckThatESPopulationCanBeCreatedAndDestroyedWithoutLeaks){
    // run this with valgrind / sanitizers
    ESPopulation **  pop = makeESPopulation();
    freeESPopulation(pop);
}

TEST_F(SresTests, CheckDoNothingTransform){
    ESfcnTrsfm** trsfm = makeTransformFun(2);

    freeTransformFun(trsfm, 2);
}


TEST_F(SresTests, CheckESInitialNoLeak){

    unsigned int seed = 1;
    int es = 1;
    int constraint = 0;
    int dim = 6;
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
    ESfcnTrsfm **trsfm = makeTransformFun(dim);

    ESInitial(
            seed,
            param,
            *trsfm,
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

    ESDeInitial(param, population, stats);
    freeTransformFun(trsfm, 2);

}

TEST_F(SresTests, CheckESInitialWithStepNoLeaks){

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
    ESfcnTrsfm **trsfm = makeTransformFun(dim);

    ESInitial(
            seed,
            param,
            *trsfm,
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
    int stepCount = 0;
    while (true){
        ESStep(population, param, stats, pf);
        stepCount++;
        if (stepCount == ngen){
            break;
        }
    }

    ESDeInitial(param, population, stats);
    freeTransformFun(trsfm, 2);
}






















