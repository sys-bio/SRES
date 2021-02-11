#include "SRES.h"
#include <cmath>
#include <iostream>
#include "random"

#define LOGSPACE true

using namespace opt;

int N = 10;


/**
 * minimum = f(3, 0.5) = 0
 */
double BealeFunction(double x, double y) {
#ifdef LOGSPACE
    x = pow(10, x);
    y = pow(10, y);
#endif
    double first = pow(1.5 - x + x*y, 2);
    double second = pow(2.25 - x + x*pow(y, 2), 2);
    double third = pow(2.625 - x + x*pow(y, 3), 2);
    return first + second + third;
};


double cost(double *input_params) {
//    std::cout << "from cost fun: log x: " << input_params[0] << "; log y: " << input_params[1] << std::endl;
//    std::cout << "from cost fun:     x:" << pow(10, input_params[0]) << ";    y: " << pow(10, input_params[1]) << std::endl;
//    return BealeFunction(pow(10, input_params[0]), pow(10, input_params[1]));
    return BealeFunction(input_params[0], input_params[1]);
}


int main() {

    SRES sres(cost, 10, 5,
              {8.324, 7.335},
              {0.01, 0.01}, {100, 100}, 7, 25, LOGSPACE, true);
    sres.fitLHS();

    std::cout << "Solution best fit log space: " << sres.getBestFitnessValue() << "; best solution: ";
    for (auto i: sres.getSolutionValues()) {
        std::cout << i << "\t";
    }
    std::cout << std::endl;
    std::cout << "Solution best fit lin space " << pow(10, sres.getBestFitnessValue()) << "; best solution: ";
    for (auto i: sres.getSolutionValues()) {
        std::cout << pow(10, i) << "\t";
    }
    std::cout << std::endl;

//    std::cout << "hall of fame ";
//    for (auto i: sres.getHallOfFame()) {
//        std::cout << i << "\t";
//    }
//    std::cout << std::endl;
}