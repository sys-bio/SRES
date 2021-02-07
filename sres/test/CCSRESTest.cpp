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


using namespace opt;

class CCSRESTests : public ::testing::Test {

public:

    double s[2] = {9.454, 3.556};
    double l[2] = {0.1, 0.1};
    double u[2] = {10.0, 10.0};

    CCSRESTests() = default;

};


TEST_F(CCSRESTests, TestConstructible1) {
    ASSERT_NO_THROW(
           SRES *sres = SRES_newSRES(cost, 10, 50, s, l, u, 2, 7);
            SRES_deleteSRES(sres);
    );
}


TEST_F(CCSRESTests, TestGetBestValue) {
    SRES *sres = SRES_newSRES(cost, 10, 50, s, l, u, 2, 7);
    SRES_setSeed(sres, 4);
    SRES_fit(sres);
    double best = SRES_getBestFitnessValue(sres);
    double x = 8.0722599999999995e-11;
    ASSERT_NEAR(x, best, 0.001);
    SRES_deleteSRES(sres);
}

TEST_F(CCSRESTests, TestGetSizeOfHOF) {
    // todo hof only gets updated when a generation finds a better individual than has been seed before.
    // Therefore, it would be better to also have an array storing the generation of when the individual
    // achieved their glory.
    SRES *sres = SRES_newSRES(cost, 10, 50, s, l, u, 2, 7);
    SRES_setSeed(sres, 4);
    SRES_fit(sres);
    ASSERT_EQ(12, SRES_getSizeOfHallOfFame(sres));
    SRES_deleteSRES(sres);
}

TEST_F(CCSRESTests, TestHallOfFame) {
    SRES *sres = SRES_newSRES(cost, 10, 100, s, l, u, 2,7);
    SRES_setSeed(sres, 4);
    SRES_fit(sres);

    double* hof = SRES_getHallOfFame(sres);
    // if we find a value in hof that is greater than the previous value then something is very wrong
    // with our algorithm
    bool alwaysDecreasing = true;
    for (int i = 1; i < SRES_getSizeOfHallOfFame(sres); i++) {
        if (hof[i - 1] < hof[i]) { // if the previous is less that the current, then fitness value has got worse
            alwaysDecreasing = false;
            break;
        }
    }
    ASSERT_TRUE(alwaysDecreasing);
    SRES_deleteSRES(sres);
}


TEST_F(CCSRESTests, TestGetSolutionValues) {
    SRES *sres = SRES_newSRES(cost, 20, 50, s, l, u, 2, 7);

    SRES_setSeed(sres, 4);
    SRES_fit(sres);

    double* sol = SRES_getSolution(sres);

    for (int i=0 ; i<2; i++){
        std::cout << "solution: parameter "<< sol[i] << std::endl;
    }

    double x = 3.0;
    double y = 0.5;
    ASSERT_NEAR(x, sol[0], 0.01);
    ASSERT_NEAR(y, sol[1], 0.01);
    SRES_deleteSRES(sres);
}
















