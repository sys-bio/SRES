#include "SRES.h"
#include <cmath>
#include <iostream>
#include "random"

using namespace csres;

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

    SRES sres(cost, 10, 50, {8.324, 7.335}, {0.1, 0.1}, {10, 10}, 7);
    sres.fit();

    std::cout << "Solution best fit: " << sres.getBestValue() << "; best solution: ";
    for (auto i: sres.getSolutionValues()){
        std::cout << i << "\t" ;
    }
    std::cout << std::endl;
}