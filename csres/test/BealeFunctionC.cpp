#include "CSRES.h"
#include <cmath>
#include <iostream>
#include "random"

using namespace opt;

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



double cost(double* input_params) {
    return BealeFunction(input_params[0], input_params[1]);

}



int main() {

    double s[2] = {9.454, 3.556};
    double l[2] = {0.1, 0.1};
    double u[2] = {10.0, 10.0};
    SRES* sres = SRES_newSRES(cost, 25, 15, s, l, u, 2, 7);
    SRES_fit(sres);

    std::cout << "Solution best fit: " << sres->getBestFitnessValue() << "; best solution: ";
    for (auto i: sres->getSolutionValues()){
        std::cout << i << "\t" ;
    }
    std::cout << std::endl;

    double* hof = SRES_getHallOfFame(sres);

    for (int i=0; i< sres->getNumGenerations(); i++){
        std::cout << hof[i] << std::endl;

    }
}