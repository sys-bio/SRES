//
// Created by Ciaran on 10/02/2021.
//


#include "gtest/gtest.h"
#include "RandomNumberGenerator.h"

using namespace opt;

class RNGTests : public ::testing::Test {

public:
    RNGTests() = default;

};


TEST_F(RNGTests, CheckThatTheShapeOfLHSMatrixIsCorrect){

    RandomNumberGenerator generator = RandomNumberGenerator::getInstance();
    int npop = 10;
    int nparam = 4;
    std::vector<std::vector<double>> pop = generator.lhs(npop , nparam);

    for (int i=0; i<npop; i++){
        for (int j=0; j<nparam; j++){
            std::cout << pop[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    ASSERT_EQ(pop.size(), 10);
    ASSERT_EQ(pop[0].size(), 4);

    // do a test for size
    // make sure each row isn't the same scale (harder)
}
TEST_F(RNGTests, CheckThatThereAreNoDuplicateNumbers){

    RandomNumberGenerator generator = RandomNumberGenerator::getInstance();
    int npop = 10;
    int nparam = 4;
    std::vector<std::vector<double>> pop = generator.lhs(npop , nparam);
    std::vector<std::vector<double>> one = generator.lhs(npop , nparam);
    std::vector<std::vector<double>> two = generator.lhs(npop , nparam);

    // make copies because we'll need to mutate these inplace
    // (sorting) in order to check for duplicates
    one = pop;
    two = pop;

    for (int i=0; i<npop; i++){
        for (int j=0; j<nparam; j++){
            std::cout << pop[i][j] << ", ";
        }
        std::cout << std::endl;
    }
    // check for uniqueness along the column direction
    for (int j=0; j < npop; j++){
        sort(one[j].begin(), one[j].end());
        auto it = std::unique(one[j].begin(), one[j].end() );
        bool wasUnique = (it == one[j].end() );
        ASSERT_TRUE(wasUnique);
    }

    // check for uniqueness along the row direction
    for (int i=0; i < nparam; i++){
        sort(two[i].begin(), two[i].end());
        auto it = std::unique(two[i].begin(), two[i].end() );
        bool wasUnique = (it == two[i].end() );
        ASSERT_TRUE(wasUnique);
    }

}