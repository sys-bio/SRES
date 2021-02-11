#include <cmath>
#include "gtest/gtest.h"
#include "SRES.h"

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


class SRESTests : public ::testing::Test {

public:
    SRESTests() = default;

};

TEST_F(SRESTests, TestFindStartingSet) {
    SRES sres(cost, 10, 5,
              {8.324, 7.335},
              {0.01, 0.01}, {100, 100}, 7, 25, LOGSPACE, true,
              10, 3);

    sres.findStartingSet();
}


TEST_F(SRESTests, TestFindBestIndividual) {
    SRES sres(cost, 10, 5,
              {8.324, 7.335},
              {0.01, 0.01}, {100, 100}, 7, 25, LOGSPACE, true,
              10, 3);
    sres.setSeed(4);
    sres.fit();
//    DoubleMatrix pop = sres.getPopulation();

//    size_t idx = sres.findBestIndividual();
//
//
//    sres;
}


TEST_F(SRESTests, Fit) {
    SRES sres(cost, 50, 50,
              {8.324, 7.335},
              {0.01, 0.01}, {100, 100}, 7, 25, LOGSPACE, true);
    sres.fit();
    auto sol = sres.getSolutionValues();
//    ASSERT_NEAR(3.0, sol[0], 0.01);
//    ASSERT_NEAR(0.5, sol[1], 0.01);
}











































