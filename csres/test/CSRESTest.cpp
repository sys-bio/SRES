#include "gtest/gtest.h"
#include "SRES.h"

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


using namespace csres;

class CSRESTests : public ::testing::Test {

public:
    CSRESTests() = default;

};


TEST_F(CSRESTests, TestConstructible1) {
    std::vector<double>s({9.454, 3.556});
    std::vector<double>l({0.1, 0.1});
    std::vector<double>u({10.0, 10.0});
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
    SRES sres(cost, 10, 50, {9.454, 3.556}, {0.1, 0.1}, {10.0, 10.0}, 7);
    sres.setSeed(4);
    sres.fit();
    auto sol = sres.getSolutionValues();
    double x = 2.9999969270322402;
    double y = 0.50000109127881553;
    ASSERT_NEAR(x, sol[0], 0.0000001);
    ASSERT_NEAR(y, sol[1], 0.0000001);
}

TEST_F(CSRESTests, TestGetBestValue) {
    SRES sres(cost, 10, 50, {9.454, 3.556}, {0.1, 0.1}, {10.0, 10.0}, 7);
    sres.setSeed(4);
    sres.fit();
    auto best = sres.getBestValue();
    double x = 8.0722599999999995e-11;
    // dont want to hard code the answer.
    // todo use mocking
//    ASSERT_NEAR(x, best, 0.000001);
}



TEST_F(CSRESTests, TestGetTrace) {
    SRES sres(cost, 10, 50, {9.454, 3.556}, {0.1, 0.1}, {10.0, 10.0}, 7);
    sres.setSeed(4);
    sres.fit();
    auto trace = sres.getTrace();
    double x = 8.0722599999999995e-11;
//    ASSERT_NEAR(x, best, 0.000001);
}















