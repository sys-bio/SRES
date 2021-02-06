#include "gtest/gtest.h"
#include "SRES.h"

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


using namespace opt;

class CSRESTests : public ::testing::Test {

public:
    CSRESTests() = default;

};


TEST_F(CSRESTests, TestConstructible1) {
    std::vector<double> s({9.454, 3.556});
    std::vector<double> l({0.1, 0.1});
    std::vector<double> u({10.0, 10.0});
    ASSERT_NO_THROW(
            SRES sres(cost, 10, 50, s, l, u, 7);
    );
}

TEST_F(CSRESTests, TestConstructibleFromVectors) {
    ASSERT_NO_THROW(
            SRES sres(cost, 10, 50, {9.454, 3.556}, {0.1, 0.1}, {10.0, 10.0}, 7);
    );
}

TEST_F(CSRESTests, TestGetSolutionValues) {
    SRES sres(cost, 20, 50, {9.454, 3.556}, {0.1, 0.1}, {10.0, 10.0}, 7);
    sres.setSeed(4);
    sres.fit();
    auto sol = sres.getSolutionValues();
    double x = 3.0;
    double y = 0.5;
    ASSERT_NEAR(x, sol[0], 0.01);
    ASSERT_NEAR(y, sol[1], 0.01);
}

TEST_F(CSRESTests, TestGetBestValue) {
    SRES sres(cost, 10, 50, {9.454, 3.556}, {0.1, 0.1}, {10.0, 10.0}, 7);
    sres.setSeed(4);
    sres.fit();
    auto best = sres.getBestFitnessValue();
    double x = 8.0722599999999995e-11;
    ASSERT_NEAR(x, best, 0.0001);
}

TEST_F(CSRESTests, TestGetHallOfFame) {
    SRES sres(cost, 10, 50, {9.454, 3.556}, {0.1, 0.1}, {10.0, 10.0}, 7);
    sres.setSeed(4);
    sres.fit();
    auto hof = sres.getHallOfFame();
    // if we find a value in hof that is greater than the previous value then something is very wrong
    // with our algorithm
    bool alwaysDecreasing = true;
    for (int i=1; i<hof.size(); i++){
        std::cout <<"Hof element "<< i <<  " : " << hof[i] << std::endl;
        if (hof[i] > hof[i-1]){ // if the present value is bigger than the previous
            std::cout << "condition triggered, setting alwaysDecreasing to false" << std::endl;
            alwaysDecreasing = false;
            break;
        }
    }
    ASSERT_TRUE(alwaysDecreasing);
}

















