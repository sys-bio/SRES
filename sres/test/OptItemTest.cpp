//
// Created by Ciaran on 05/02/2021.
//

#include "gtest/gtest.h"
#include "OptItem.h"

using namespace opt;
class OptItemTests : public ::testing::Test {

public:
    OptItemTests() = default;

};


TEST_F(OptItemTests, TestConstructible){
    ASSERT_NO_THROW(
            OptItem item(0.5, 0.1, 10, false);
    );
}

TEST_F(OptItemTests, CheckLbIsLogged){
    OptItem item(0.5, 0.1, 10, true);
    ASSERT_EQ(-1, item.getLb());
}

