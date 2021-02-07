//
// Created by Ciaran on 05/02/2021.
//

#include "gtest/gtest.h"
#include "OptItems.h"

using namespace opt;

class OptItemsTests : public ::testing::Test {

public:
    OptItemsTests() = default;

};


TEST_F(OptItemsTests, TestConstructibleFromVectors) {
    ASSERT_NO_THROW(
            OptItems items({0.4, 0.5}, {0.1, 0.1}, {10.0, 10.0})
    );
}















