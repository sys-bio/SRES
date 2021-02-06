#include "gtest/gtest.h"
#include "CSRES.h"

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

class CCSRESTests : public ::testing::Test {

public:
    CCSRESTests() = default;

};


TEST_F(CCSRESTests, TestConstructible1) {
    double s[2] = {9.454, 3.556};
    double l[2] = {0.1, 0.1};
    double u[2] = {10.0, 10.0};
    ASSERT_NO_THROW(
            SRES *sres = SRES_newSRES(cost, 10, 50, s, l, u, 2, 7);
    );
}


TEST_F(CCSRESTests, TestGetSolutionValues) {
    double s[2] = {9.454, 3.556};
    double l[2] = {0.1, 0.1};
    double u[2] = {10.0, 10.0};
    SRES *sres = SRES_newSRES(cost, 10, 50, s, l, u, 2, 7);

    SRES_setSeed(sres, 4);
    SRES_fit(sres);
    auto sol = SRES_getSolutionValues(sres);
    double x = 2.9999969270322402;
    double y = 0.50000109127881553;
    ASSERT_NEAR(x, sol[0], 0.0000001);
    ASSERT_NEAR(y, sol[1], 0.0000001);
    SRES_freeSolutionValues(sol);
}

TEST_F(CCSRESTests, TestGetBestValue) {
    double s[2] = {9.454, 3.556};
    double l[2] = {0.1, 0.1};
    double u[2] = {10.0, 10.0};
    SRES *sres = SRES_newSRES(cost, 10, 50, s, l, u, 2, 7);

    SRES_setSeed(sres, 4);
    SRES_fit(sres);
    auto best = SRES_getBestValue(sres);
    double x = 8.0722599999999995e-11;
//    ASSERT_NEAR(x, *best, 0.000001);
    SRES_freeBestValue(best);
}
















