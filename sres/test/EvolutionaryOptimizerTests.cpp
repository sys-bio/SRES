#include <cmath>
#include "gtest/gtest.h"

/**
 * note: we need a base class of SRES to mock the algorithm in testing.
 * We need an optimizer base and a population base.
 */

/**
 * minimum = f(3, 0.5) = 0
 */
double BealeFunction(double x, double y) {
    double first = pow(1.5 - x + x * y, 2);
    double second = pow(2.25 - x + x * pow(y, 2), 2);
    double third = pow(2.625 - x + x * pow(y, 3), 2);
    return first + second + third;
};


double cost(double *input_params) {
    return BealeFunction(input_params[0], input_params[1]);
}



class EvolutionaryOptimizerTests : public ::testing::Test {

public:
    EvolutionaryOptimizerTests() = default;

};












